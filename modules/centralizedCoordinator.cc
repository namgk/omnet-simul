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
//        virtual void RunExample();
};

Define_Module(CentralizedCoordinator);

namespace operations_research {

void CPSimple(CentralizedCoordinator *coordinator) {
    Solver solver("CPSimple");
    const int64 numVals = coordinator->numberOfMobileHost;// 5 mobile hosts

    IntVar* const x = solver.MakeIntVar(0, numVals - 1, "x");
    IntVar* const y = solver.MakeIntVar(0, numVals - 1, "y");
    IntVar* const z = solver.MakeIntVar(0, numVals - 1, "z");
    IntVar* const w = solver.MakeIntVar(0, numVals - 1, "w");

    IntVar* const p = solver.MakeIntVar(0, 20, "pheasant");
    IntVar* const r = solver.MakeIntVar(0, 20, "rabbit");
    IntExpr* const region = solver.MakeProd(p, 2);
    IntExpr* const heads = solver.MakeSum(p, r);
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
    for (cModule::SubmoduleIterator it(parent); !it.end(); it++)
    {
      cModule *submodule = *it;
      const char hostName[] = "mobileHost";
      const char *subName = submodule->getName();
      if (strcmp (hostName,subName) == 0){
          EV << subName << endl;
          cModule *mobility = submodule->getSubmodule("mobility");
          if (!mobility){
              return;
          }

          inet::MassMobility *mob = check_and_cast<inet::MassMobility *>(mobility);
          try {
              inet::Coord coord = mob->getCurrentPosition();
              std::string coordInfo = coord.info();
              EV_INFO << " _________ " << coordInfo;
          } catch( std::exception e ) {
          }
      }
    }
    scheduleAt(simTime()+par("coordinationPeriod").doubleValue(), sendMessageEvent);
}
