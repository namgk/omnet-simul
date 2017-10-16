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

using namespace omnetpp;

class CentralizedCoordinator : public cSimpleModule
{
    const char *name;
    cModule *parent;

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
        void RunTest();
};

namespace operations_research {

void CPSimple(CentralizedCoordinator *coordinator) {
    Solver solver("CPSimple");

    // for all locations[x] of hosts that can run 'a'
    std::vector<int64> a_hosts_x = {};
    a_hosts_x.push_back(121); // host[1].x = 121
    a_hosts_x.push_back(322); // host[3].x = 322
    a_hosts_x.push_back(49); // host[9].x = 49

    // for all locations[y] of hosts that can run 'a'
    std::vector<int64> a_hosts_y = {};
    a_hosts_y.push_back(31); // host[1].y = 31
    a_hosts_y.push_back(22); // host[3].y = 22
    a_hosts_y.push_back(249); // host[9].y = 249

    const int64 numVals = a_hosts_x.size();
    IntVar* const a = solver.MakeIntVar(0, numVals - 1);

    // turn a into x: function of a_instance
    // x_a = a_hosts_x[a_instance]
    IntExpr* const x_a = solver.MakeElement(a_hosts_x, a);

//    Constraint* const ct_legs = solver.MakeEquality(legs, 56);
}

}   // namespace operations_research

CentralizedCoordinator::CentralizedCoordinator()
{
    sendMessageEvent = nullptr;
}

CentralizedCoordinator::~CentralizedCoordinator()
{
    cancelAndDelete(sendMessageEvent);
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

    RunTest();
    operations_research::CPSimple(this);

    sendMessageEvent = new cMessage("sendMessageEvent");
    scheduleAt(simTime(), sendMessageEvent);
}

void CentralizedCoordinator::RunTest() {
    operations_research::MPSolver solver("Glop", operations_research::MPSolver::GLOP_LINEAR_PROGRAMMING);
    operations_research::MPVariable* const x = solver.MakeNumVar(0.0, 1, "x");
    operations_research::MPVariable* const y = solver.MakeNumVar(0.0, 2, "y");
    operations_research::MPObjective* const objective = solver.MutableObjective();
    objective->SetCoefficient(x, 1);
    objective->SetCoefficient(y, 1);
    objective->SetMaximization();
    solver.Solve();

//    printf("\nx = %.1f", x->solution_value());
//    printf("\ny = %.1f", y->solution_value());
}

void CentralizedCoordinator::handleMessage(cMessage *msg)
{
    operations_research::Solver solver("CPSimple");

    // for all locations[x] of hosts that can run 'a'
    std::vector<int64> a_hosts_x = {};
    std::vector<int64> a_hosts_y = {};
    std::vector<int64> b_hosts_x = {};
    std::vector<int64> b_hosts_y = {};

    for (cModule::SubmoduleIterator it(parent); !it.end(); it++)
    {
      cModule *submodule = *it;
      const char hostName[] = "mobileHost";
      const char *subName = submodule->getName();
      if (strcmp (hostName,subName) == 0){
          // mobile
          EV << subName << endl;
          cModule *mobility = submodule->getSubmodule("mobility");
          if (!mobility){
              continue;
          }

          try {
              inet::MassMobility *mob = check_and_cast<inet::MassMobility *>(mobility);
              inet::Coord coord = mob->getCurrentPosition();
              std::string coordInfo = coord.info();
              a_hosts_x.push_back(static_cast<int64>(coord.x));
              a_hosts_y.push_back(static_cast<int64>(coord.y));
//              EV_INFO << " _________MOBILE " << coordInfo;
          } catch( std::exception e ) {
          }
      } else {
          // immobile
          cModule *mobility = submodule->getSubmodule("mobility");
          try {
              inet::StationaryMobility *mob = check_and_cast<inet::StationaryMobility *>(mobility);
              inet::Coord coord = mob->getCurrentPosition();
              std::string coordInfo = coord.info();
//              EV_INFO << " _________IMMOBILE " << coordInfo;

              b_hosts_x.push_back(static_cast<int64>(coord.x));
              b_hosts_y.push_back(static_cast<int64>(coord.y));
          } catch( std::exception e ) {
          }
      }
    }

    const int64 aHosts = a_hosts_x.size();
    const int64 bHosts = b_hosts_x.size();
    operations_research::IntVar* const a = solver.MakeIntVar(0, aHosts - 1);
    operations_research::IntVar* const b = solver.MakeIntVar(0, bHosts - 1);

    std::vector<operations_research::IntVar*> allvars = {};
    allvars.push_back(a);
    allvars.push_back(b);

    // turn a into x: function of a_instance
    // x_a = a_hosts_x[a_instance]
    operations_research::IntExpr* const x_a = solver.MakeElement(a_hosts_x, a);
    operations_research::IntExpr* const x_b = solver.MakeElement(b_hosts_x, b);

    operations_research::IntExpr* a_b_x_abs_difference = solver.MakeAbs(solver.MakeDifference(x_a, x_b));
    operations_research::Constraint* nearby = solver.MakeLessOrEqual(a_b_x_abs_difference, 100);

    solver.AddConstraint(nearby);
    operations_research::DecisionBuilder* const db = solver.MakePhase(allvars,
            operations_research::Solver::CHOOSE_FIRST_UNBOUND,
            operations_research::Solver::ASSIGN_MIN_VALUE);

    solver.Solve(db);
    int count = 0;

    while (solver.NextSolution()) {
        count++;
    }

    printf("\nNumber of solutions:%d", count);

    scheduleAt(simTime()+par("coordinationPeriod").doubleValue(), sendMessageEvent);
}


Define_Module(CentralizedCoordinator);
