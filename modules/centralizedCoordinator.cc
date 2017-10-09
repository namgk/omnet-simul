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

using namespace omnetpp;

class CentralizedCoordinator : public cSimpleModule
{
    const char *name;
    private:
        cMessage *sendMessageEvent;
    public:
        CentralizedCoordinator();
        virtual ~CentralizedCoordinator();
    protected:
        // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void RunTest(operations_research::MPSolver::OptimizationProblemType optimization_problem_type);
//        virtual void RunExample();
};

Define_Module(CentralizedCoordinator);

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
//    EV_INFO << " >>>> " << name;

    RunTest();

    sendMessageEvent = new cMessage("sendMessageEvent");
    scheduleAt(simTime(), sendMessageEvent);
}

void CentralizedCoordinator::RunTest(
    operations_research::MPSolver::OptimizationProblemType optimization_problem_type) {
    operations_research::MPSolver solver("Glop", operations_research::MPSolver::GLOP_LINEAR_PROGRAMMING);
    operations_research::MPVariable* const x = solver.MakeNumVar(0.0, 1, "x");
    operations_research::MPVariable* const y = solver.MakeNumVar(0.0, 2, "y");
    operations_research::MPObjective* const objective = solver.MutableObjective();
    objective->SetCoefficient(x, 1);
    objective->SetCoefficient(y, 1);
    objective->SetMaximization();
    solver.Solve();

    EV_INFO << " Solution ";
    EV_INFO << " Solution " << x->solution_value();

//    printf("\nx = %.1f", x->solution_value());
//    printf("\ny = %.1f", y->solution_value());
  }
//
//void CentralizedCoordinator::RunExample() {
//    RunTest();
//}

void CentralizedCoordinator::handleMessage(cMessage *msg)
{
    scheduleAt(simTime()+par("sendIaTime").doubleValue(), sendMessageEvent);
}


