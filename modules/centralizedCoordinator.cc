//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <string.h>
#include <omnetpp.h>
#include "inet/common/geometry/common/Coord.h"
#include "inet/mobility/single/MassMobility.h"
#include "inet/mobility/static/StationaryMobility.h"
#include "ortools/linear_solver/linear_solver.h"
#include "ortools/linear_solver/linear_solver.pb.h"
#include "ortools/constraint_solver/constraint_solveri.h"

#include "ContextSync.h"

using namespace omnetpp;

class CentralizedCoordinator : public cSimpleModule
{
    const char *name;
    cModule *parent;
    cArray *participant;

    private:
        cMessage *sendMessageEvent;
    public:
        double numberOfMobileHost;
        CentralizedCoordinator();
        virtual ~CentralizedCoordinator();
    protected:
        // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};


CentralizedCoordinator::CentralizedCoordinator()
{
    sendMessageEvent = nullptr;
}

CentralizedCoordinator::~CentralizedCoordinator()
{
    cancelAndDelete(sendMessageEvent);
    delete participant;
}

void CentralizedCoordinator::initialize()
{
    name = getName();//coordinator
    EV_INFO << " >>>> " << name;

    parent = getParentModule(); // whole network
    if (!parent){
        return;
    }

    numberOfMobileHost = parent->par("numHosts").doubleValue();

    participant = new cArray("participant", 100, 10);

    sendMessageEvent = new cMessage("sendMessageEvent");
    scheduleAt(simTime(), sendMessageEvent);
}

void CentralizedCoordinator::handleMessage(cMessage *msg)
{
    if (msg != sendMessageEvent){
        cObject *payload = msg->getObject("ctx");
        ContextSync *ctx = check_and_cast<ContextSync *>(payload);
        const char *device = ctx->getDevice();
//        double x = ctx->getX();
//        double y = ctx->getY();
//        ctx->setName(device);
        msg->setName(device);
        participant->set(msg);
        return;
    }

    scheduleAt(simTime()+par("coordinationPeriod").doubleValue(), sendMessageEvent);

    if (participant->size() != numberOfMobileHost){
        return;
    }

    operations_research::Solver solver("CPSimple");

    std::vector<ContextSync*> a_hosts = {};
    std::vector<int64> a_hosts_x = {};
    std::vector<int64> a_hosts_y = {};
    std::vector<ContextSync*> b_hosts = {};
    std::vector<int64> b_hosts_x = {};
    std::vector<int64> b_hosts_y = {};

    for (int i = 0; i < participant->size(); i++) {
        cMessage *savedMsg = (cMessage *) participant->get(i);
        ContextSync *obj = check_and_cast<ContextSync *>(savedMsg->getObject("ctx"));
        a_hosts_x.push_back(static_cast<int64>(obj->getX()));
        a_hosts_y.push_back(static_cast<int64>(obj->getY()));
        a_hosts.push_back(obj);
    }


    for (cModule::SubmoduleIterator it(parent); !it.end(); it++)
    {
      cModule *submodule = *it;
      const char hostName[] = "mobileHost";
      const char *subName = submodule->getName();
      if (strcmp (hostName,subName) != 0){
          // immobile, fog, cloud
          cModule *mobility = submodule->getSubmodule("mobility");
          try {
              inet::StationaryMobility *mob = check_and_cast<inet::StationaryMobility *>(mobility);
              inet::Coord coord = mob->getCurrentPosition();
              std::string coordInfo = coord.info();

              b_hosts_x.push_back(static_cast<int64>(coord.x));
              b_hosts_y.push_back(static_cast<int64>(coord.y));
              ContextSync *bCtx = new ContextSync();
              bCtx->setDevice(submodule->getFullName());
              b_hosts.push_back(bCtx);
          } catch( std::exception e ) {
          }
      }
    }

    operations_research::IntVar* const a = solver.MakeIntVar(0, a_hosts_x.size() - 1);
    operations_research::IntVar* const b = solver.MakeIntVar(0, b_hosts_x.size() - 1);

    std::vector<operations_research::IntVar*> allvars = {};
    allvars.push_back(a);
    allvars.push_back(b);

    // turn a into x: function of a
    // x_a = a_hosts_x[a]
    operations_research::IntExpr* const x_a = solver.MakeElement(a_hosts_x, a);
    operations_research::IntExpr* const x_b = solver.MakeElement(b_hosts_x, b);

    operations_research::IntExpr* a_b_x_abs_difference = solver.MakeAbs(solver.MakeDifference(x_a, x_b));
    operations_research::Constraint* nearby = solver.MakeLessOrEqual(a_b_x_abs_difference, 30);

    solver.AddConstraint(nearby);
    operations_research::DecisionBuilder* const db = solver.MakePhase(allvars,
            operations_research::Solver::CHOOSE_FIRST_UNBOUND,
            operations_research::Solver::ASSIGN_MIN_VALUE);

    operations_research::SolutionCollector *collector = solver.MakeAllSolutionCollector();
    collector->Add(a);
    collector->Add(b);

    if (solver.Solve(db, solver.MakeTimeLimit(500), collector)){
        int numOfSolutions = collector->solution_count();
        printf("\nNumber of solutions: %d", numOfSolutions);

        std::map<std::string, int> dupCheck;
        std::vector<std::array<std::string, 2>> coordinationResults = {};

        for (int i = 0; i < numOfSolutions; i++){

            std::array<std::string, 2> result;

            const char *aAssignment = a_hosts.at(collector->Value(i, a))->getDevice();
            const char *bAssignment = b_hosts.at(collector->Value(i, b))->getDevice();
            printf("\na,b: %s %s\n", aAssignment,  bAssignment);

            std::string aStr(aAssignment);
            std::string bStr(bAssignment);

            if (dupCheck.find(aStr) == dupCheck.end() &&
                    dupCheck.find(bStr) == dupCheck.end()){
                dupCheck.insert(std::pair<std::string, int>(aStr, 1));
                dupCheck.insert(std::pair<std::string, int>(bStr, 1));
                result = {aStr, bStr};
                coordinationResults.push_back(result);
            } else {
                continue;
            }
        }
        std::cout << "\nNumber of coordination solutions: " << coordinationResults.size();
    }
}


Define_Module(CentralizedCoordinator);
