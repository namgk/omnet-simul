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
#include "inet/mobility/static/StationaryMobility.h"
#include "ortools/linear_solver/linear_solver.h"
#include "ortools/linear_solver/linear_solver.pb.h"
#include "ortools/constraint_solver/constraint_solveri.h"

#include "ContextSync.h"
#include "SignalListener.h"
#include "CoordinationResult.h"


using namespace omnetpp;

class ResultCollector : public cSimpleModule
{
    SignalListener *listener;
    simsignal_t sentRecvSignal;
    simsignal_t droppedSignal;
    simsignal_t totalSolutionSentSignal;

    private:
        cMessage *sendMessageEvent;
        cMessage *resultCollectingEvent;
    public:
        ResultCollector();
        virtual ~ResultCollector();
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};


ResultCollector::ResultCollector()
{
    sendMessageEvent = nullptr;
    resultCollectingEvent = nullptr;
}

ResultCollector::~ResultCollector()
{
    cancelAndDelete(sendMessageEvent);
    cancelAndDelete(resultCollectingEvent);
}

void ResultCollector::initialize()
{

    sentRecvSignal = registerSignal("sentrecv");
    droppedSignal = registerSignal("dropped");
    totalSolutionSentSignal = registerSignal("totalSolution");

    // Event listener for result collecting
    listener = new SignalListener();
    getSimulation()->getSystemModule()->subscribe("sent", listener);
    getSimulation()->getSystemModule()->subscribe("recv", listener);
    getSimulation()->getSystemModule()->subscribe("dropped", listener);
    getSimulation()->getSystemModule()->subscribe("solutionsSent", listener);

    sendMessageEvent = new cMessage("sendMessageEvent");
    resultCollectingEvent = new cMessage("resultCollectingEvent");
    scheduleAt(simTime(), sendMessageEvent);
    scheduleAt(simTime(), resultCollectingEvent);
}

void ResultCollector::handleMessage(cMessage *msg)
{
    // collect coordination result
    if (msg == sendMessageEvent){
        scheduleAt(simTime()+par("coordinationPeriod").doubleValue(), sendMessageEvent);
        emit(totalSolutionSentSignal, listener->getSolutionSentMsg());
        listener->setSolutionSentMsg(0);
    }

    // collect sent/recv statistic
    if (msg == resultCollectingEvent){
        scheduleAt(simTime()+par("sendIaTime").doubleValue(), resultCollectingEvent);
        // broadcast sent/received ratio and reset counter
        if (listener->getSent() != 0){
            emit(sentRecvSignal, listener->getRecv()*100/listener->getSent());
            emit(droppedSignal, listener->getDroppedMsg()*100/listener->getSent());
            listener->setSent(0);
            listener->setRecv(0);
            listener->setDroppedMsg(0);
        }
    }

}


Define_Module(ResultCollector);
