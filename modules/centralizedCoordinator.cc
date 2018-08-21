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
#include <cmath>

#include "inet/common/geometry/common/Coord.h"
#include "inet/mobility/static/StationaryMobility.h"
#include "ortools/linear_solver/linear_solver.h"
#include "ortools/linear_solver/linear_solver.pb.h"
#include "ortools/constraint_solver/constraint_solveri.h"

#include "ContextSync.h"
#include "Assignment.h"
#include "CoordinationResult.h"


using namespace omnetpp;

class CentralizedCoordinator : public cSimpleModule
{
    const char *name;
    cModule *parent;
    cArray *participant;
    cArray *pastAssignmentsByA;
    cArray *pastAssignmentsByC;
    cArray *bTobeSkipped;
    double speed;
    int myNumber;

    simsignal_t solutionsSent;
    simsignal_t aReused;
    simsignal_t bReused;
    simsignal_t cReused;

    private:
        cMessage *sendMessageEvent;
    public:
        double numberOfMobileHost;
        int numberOfCoord;
        int nearbyDef;
        int solverTimeLimit;
        CentralizedCoordinator();
        virtual ~CentralizedCoordinator();
    protected:
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
    delete bTobeSkipped;
    delete pastAssignmentsByA;
    delete pastAssignmentsByC;
}

void CentralizedCoordinator::initialize()
{
    name = getName();//coordinator
    EV_INFO << " >>>> " << name;

    parent = getParentModule(); // whole network
    if (!parent){
        return;
    }

    solutionsSent = registerSignal("solutionsSent");
    aReused = registerSignal("aReused");
    bReused = registerSignal("bReused");
    cReused = registerSignal("cReused");

    numberOfMobileHost = parent->par("numHosts").doubleValue();
    numberOfCoord = parent->par("numCoord").operator int();
    nearbyDef = parent->par("nearbyDef").operator int();
    solverTimeLimit = parent->par("solverTimeLimit").operator int();

    participant = new cArray("participant", numberOfMobileHost*2/numberOfCoord, 100);
    pastAssignmentsByA = new cArray("pastAssignmentsA", numberOfMobileHost/numberOfCoord, 100);
    pastAssignmentsByC = new cArray("pastAssignmentsC", numberOfMobileHost/numberOfCoord, 100);
    bTobeSkipped = new cArray("bTobeSkipped", numberOfMobileHost/numberOfCoord, 100);

    const char *device = getFullName();
    const char *openSquareBracket = strchr(device, '[') + 1;
    const char *closeSquareBracket = strchr(device, ']');
    int length = closeSquareBracket - openSquareBracket;
    char deviceName[length+1];
    strncpy(deviceName, openSquareBracket, length);
    deviceName[length] = '\0';
    std::string deviceStr(deviceName);
    myNumber = std::stoi(deviceStr);

    sendMessageEvent = new cMessage("sendMessageEvent");
    scheduleAt(simTime(), sendMessageEvent);
}

void CentralizedCoordinator::handleMessage(cMessage *msg)
{
    if (msg != sendMessageEvent){
        // context sync sent from participating devices, only from moving components
        cObject *payload = msg->getObject("ctx");
        ContextSync *ctx = check_and_cast<ContextSync *>(payload);
        const char *device = ctx->getDevice();
        msg->setName(device);
        participant->set(msg);
        return;
    }

    // periodic coordination execution
    scheduleAt(simTime()+par("coordinationPeriod").doubleValue(), sendMessageEvent);

    // to stores all hosts running 'a' components and their coordinate
    std::vector<ContextSync*> a_hosts = {};
    std::vector<int64> a_hosts_x = {};
    std::vector<int64> a_hosts_y = {};

    // to stores all hosts running b' components
    std::vector<ContextSync*> b_hosts = {};
    std::vector<int64> b_hosts_x = {};
    std::vector<int64> b_hosts_y = {};

    // to stores all hosts running 'c' components and their coordinate
    std::vector<ContextSync*> c_hosts = {};
    std::vector<int64> c_hosts_x = {};
    std::vector<int64> c_hosts_y = {};
    // fill up the component stores for moving components (a,c)
    // here we assume every mobile device runs both a and c
    for (int i = 0; i < participant->size(); i++) {
        cMessage *savedMsg = (cMessage *) participant->get(i);
        ContextSync *obj = check_and_cast<ContextSync *>(savedMsg->getObject("ctx"));
        const char* deviceName = obj->getDevice();

        // check deviceName, obj X.Y to see if they already satisfy the requirement
        // find the last peers of this deviceName: source and sink peer
        Assignment *sourcePeers = (Assignment *) pastAssignmentsByC->get(deviceName);
        Assignment *sinkPeers = (Assignment *) pastAssignmentsByA->get(deviceName);

        if (sourcePeers){
            printf("%s", sourcePeers);
            const char* sourcePeerName = sourcePeers->getA(); // a ->
            cMessage *sourceSyncMsg = (cMessage *) participant->get(sourcePeerName);
            ContextSync *sourceSyncCtx = check_and_cast<ContextSync *>(sourceSyncMsg->getObject("ctx"));
            double lastSourceX = sourceSyncCtx->getX();
            double lastSourceY = sourceSyncCtx->getY();
            double lastSourceDiffX = std::abs (lastSourceX - obj->getX());
            double lastSourceDiffY = std::abs (lastSourceY - obj->getY());

            if (lastSourceDiffX > nearbyDef || lastSourceDiffY > nearbyDef){
                // this obj (act as c) and its corresponding a do not meet the constraints
                c_hosts_x.push_back(static_cast<int64>(obj->getX()));
                c_hosts_y.push_back(static_cast<int64>(obj->getY()));
                c_hosts.push_back(obj);
            } else {
                emit(cReused, 1);
                const char* componentPeerName = sourcePeers->getB();
                cMessage *componentSyncMsg = (cMessage *) participant->get(componentPeerName);
                if (componentSyncMsg){
                    bTobeSkipped->set(componentSyncMsg);
                }
            }
        } else {
            c_hosts_x.push_back(static_cast<int64>(obj->getX()));
            c_hosts_y.push_back(static_cast<int64>(obj->getY()));
            c_hosts.push_back(obj);
        }

        if (sinkPeers){
            const char* sinkPeerName = sinkPeers->getC(); // -> c
            cMessage *sinkSyncMsg = (cMessage *) participant->get(sinkPeerName);
            ContextSync *sinkSyncCtx = check_and_cast<ContextSync *>(sinkSyncMsg->getObject("ctx"));
            double lastSinkX = sinkSyncCtx->getX();
            double lastSinkY = sinkSyncCtx->getY();
            double lastSinkDiffX = std::abs (lastSinkX - obj->getX());
            double lastSinkDiffY = std::abs (lastSinkY - obj->getY());

            if (lastSinkDiffX > nearbyDef || lastSinkDiffY > nearbyDef){
                a_hosts_x.push_back(static_cast<int64>(obj->getX()));
                a_hosts_y.push_back(static_cast<int64>(obj->getY()));
                a_hosts.push_back(obj);
            } else {
                emit(aReused, 1);
                const char* componentPeerName = ((Assignment *) pastAssignmentsByA->get(deviceName))->getB();
                cMessage *componentSyncMsg = (cMessage *) participant->get(componentPeerName);
                if (componentSyncMsg){
                    bTobeSkipped->set(componentSyncMsg);
                }
            }
        } else {
            a_hosts_x.push_back(static_cast<int64>(obj->getX()));
            a_hosts_y.push_back(static_cast<int64>(obj->getY()));
            a_hosts.push_back(obj);
        }
    }

    // fill up the component stores for stationary components (b)
    // here we get from the centralized database as these components do not move
    // hence no context sync required
    for (cModule::SubmoduleIterator it(parent); !it.end(); it++)
    {
      cModule *submodule = *it;
      const char hostName[] = "mobileHost";
      const char *subName = submodule->getName();
      // not mobileHost
      if (strcmp (hostName,subName) != 0){
          // fog, cloud
          cModule *mobility = submodule->getSubmodule("mobility");
          if (!mobility){
              continue;
          }
          try {
              inet::StationaryMobility *mob = check_and_cast<inet::StationaryMobility *>(mobility);
              inet::Coord coord = mob->getCurrentPosition();
              std::string coordInfo = coord.info();

              const char *device = submodule->getFullName();
              const char *openSquareBracket = strchr(device, '[') + 1;
              const char *closeSquareBracket = strchr(device, ']');
              int length = closeSquareBracket - openSquareBracket;
              char deviceName[length+1];
              strncpy(deviceName, openSquareBracket, length);
              deviceName[length] = '\0';
              std::string deviceStr(deviceName);
              int deviceNumber = std::stoi(deviceStr);

              if ((deviceNumber % numberOfCoord) == myNumber ){
                  // checking if submodule with coord.x and coord.y still satisfy
                  // the requirement
                  if (bTobeSkipped->get(submodule->getFullName())){
                      emit(bReused, 1);
                      continue;
                  }
                  
                  ContextSync *bCtx = new ContextSync();
                  bCtx->setDevice(submodule->getFullName());
                  
                  b_hosts.push_back(bCtx);
                  b_hosts_x.push_back(static_cast<int64>(coord.x));
                  b_hosts_y.push_back(static_cast<int64>(coord.y));
              }
          } catch( std::exception e ) {
              printf("BUG ALERT!!!");
              printf("%s", e.what());
          }
      }
    }


    if (a_hosts_x.size() != b_hosts_x.size() && b_hosts_x.size() != c_hosts_x.size())
        return;

    // initiate constraint solver
    operations_research::Solver solver("CPSimple");

    // three variables: the a, b, c components
    operations_research::IntVar* const a = solver.MakeIntVar(0, a_hosts_x.size() - 1);
    operations_research::IntVar* const b = solver.MakeIntVar(0, b_hosts_x.size() - 1);
    operations_research::IntVar* const c = solver.MakeIntVar(0, c_hosts_x.size() - 1);

    std::vector<operations_research::IntVar*> allvars = {};
    allvars.push_back(a);
    allvars.push_back(b);
    allvars.push_back(c);

    // now setting up constraints

    // 1, setup locations of a and c
    operations_research::IntExpr* const x_a = solver.MakeElement(a_hosts_x, a);
    operations_research::IntExpr* const y_a = solver.MakeElement(a_hosts_y, a);
    operations_research::IntExpr* const x_c = solver.MakeElement(c_hosts_x, c);
    operations_research::IntExpr* const y_c = solver.MakeElement(c_hosts_y, c);

    // 2, define nearby constraint for a and c
    operations_research::IntExpr* a_c_x_abs_difference = solver.MakeAbs(solver.MakeDifference(x_a, x_c));
    operations_research::IntExpr* a_c_y_abs_difference = solver.MakeAbs(solver.MakeDifference(y_a, y_c));
    operations_research::Constraint* nearbyX = solver.MakeLessOrEqual(a_c_x_abs_difference, nearbyDef);
    operations_research::Constraint* nearbyY = solver.MakeLessOrEqual(a_c_y_abs_difference, nearbyDef);

    // 3, define different host constraint for a and c
    operations_research::Constraint* differentHost = solver.MakeNonEquality(a, c);

    // 4 a, b, c dependency constraint: just pick any b because there's no constraint on b

    solver.AddConstraint(nearbyX);
    solver.AddConstraint(nearbyY);
    solver.AddConstraint(differentHost);

    // decision configuration
    operations_research::DecisionBuilder* const db = solver.MakePhase(allvars,
            operations_research::Solver::CHOOSE_FIRST_UNBOUND,
            operations_research::Solver::ASSIGN_MIN_VALUE);

    // setup the result collector
    operations_research::SolutionCollector *collector = solver.MakeAllSolutionCollector();
    collector->Add(a);
    collector->Add(b);
    collector->Add(c);

    // solve the problem in ms time limit requirement
    if (!solver.Solve(db, solver.MakeTimeLimit(solverTimeLimit), collector)){
        return;
    }


    //  solution duplication check: this is to derive as many solution as possible
    std::map<std::string, int> dupCheck;
    std::string coordinationResultsStr = " ";

    int numOfSolutions = collector->solution_count();
    int numOfCoordSolutions = 0;
    for (int i = 0; i < numOfSolutions; i++){
        const char *aAssignment = a_hosts.at(collector->Value(i, a))->getDevice();
        const char *bAssignment = b_hosts.at(collector->Value(i, b))->getDevice();
        const char *cAssignment = c_hosts.at(collector->Value(i, c))->getDevice();

        std::string aStr(aAssignment);
        std::string bStr(bAssignment);
        std::string cStr(cAssignment);

        std::string resultStr = "";
        // if not exist in the map, add/count the result
        if (dupCheck.find('a'+aStr) == dupCheck.end() &&
                dupCheck.find('b'+bStr) == dupCheck.end() &&
                dupCheck.find('c'+cStr) == dupCheck.end()){
            dupCheck.insert(std::pair<std::string, int>('a'+aStr, 1));
            dupCheck.insert(std::pair<std::string, int>('b'+bStr, 1));
            dupCheck.insert(std::pair<std::string, int>('c'+cStr, 1));
            resultStr += aStr + "-" + bStr + "-" + cStr;
            Assignment *resultAssignment = new Assignment(aAssignment, bAssignment, cAssignment);
            pastAssignmentsByA->set(resultAssignment);
            Assignment *resultAssignmentByC = new Assignment(aAssignment, bAssignment, cAssignment);
            resultAssignmentByC->setName(cAssignment);
            pastAssignmentsByC->set(resultAssignmentByC);
            coordinationResultsStr += resultStr + " ";
            numOfCoordSolutions++;
        } else {
            continue;
        }
    }

    emit(solutionsSent, numOfCoordSolutions);
//    std::cout << "Sending assignments " << coordinationResultsStr.c_str() << "\n";

    for (int i = 0; i < gateSize("action"); i++) {
        cGate *gatee = gate("action", i);
        cMessage *coordinationAction = new cMessage();
        coordinationAction->addPar("action");
        coordinationAction->par("action").setStringValue(coordinationResultsStr.c_str());
        send(coordinationAction, gatee);
    }
}


Define_Module(CentralizedCoordinator);
