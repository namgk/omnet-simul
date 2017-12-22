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
#include "ContextSync.h"
#include "CoordinationResult.h"

//#include "inet/common/geometry/common/Coord.h"
#include "inet/mobility/single/MassMobility.h"

using namespace omnetpp;

class LocalCoordinator : public cSimpleModule
{
    const char *name;
    const char *device;
    // host
    cModule *parent;
    // network
    cModule *grandParent;
    cModule *mobility;

    private:
        cMessage *sendMessageEvent;
    public:
        LocalCoordinator();
        virtual ~LocalCoordinator();
    protected:
        // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(LocalCoordinator);

LocalCoordinator::LocalCoordinator()
{
    sendMessageEvent = nullptr;
}

LocalCoordinator::~LocalCoordinator()
{
    cancelAndDelete(sendMessageEvent);
}

void LocalCoordinator::initialize()
{
    parent = getParentModule();// host
    if (!parent){
        return;
    }
    grandParent = parent->getParentModule(); // network
    if (!grandParent){
        return;
    }

//    printf("Parent: %s\n", grandParent->getFullName());

    mobility = parent->getSubmodule("mobility");
    if (!mobility){
        return;
    }
//
//    try {
//        inet::MassMobility *mob = check_and_cast<inet::MassMobility *>(mobility);
//        inet::Coord coord = mob->getCurrentPosition();
//        std::string coordInfo = coord.info();
//        EV_INFO << " _________ " << coordInfo;
//    } catch( std::exception e ) {
//        EV_ERROR << msg << endl;
//    }



    name = getName();//coordinator
    device = parent->getFullName();
    EV_INFO << " >>>> " << device << " >>>> " << name;

    sendMessageEvent = new cMessage("sendMessageEvent");
    scheduleAt(simTime(), sendMessageEvent);
}

void LocalCoordinator::handleMessage(cMessage *msgx)
{
    if (msgx != sendMessageEvent){
        const char* actions = msgx->par("action").stringValue();

        // e.g actions: mobileHost[2]-cloudHost[0]|mobileHost[3]-cloudHost[1]
        // e.g device: mobileHost[3]
        char *pch = strstr(actions, device);
        if (pch != NULL){
            int idx = pch - actions;//pointer to integer
            if (actions[idx-1] == '|'){
                // this plays 'a' role, sender, e.g mobileHost[3]
                // now, send to whom? e.g cloudHost[1]
                char *afterDash = strchr(pch, '-') + 1; // finding '-' pointer
                char *vertical = strchr(afterDash, '|');
                int l = vertical - afterDash;
                char destination[l+1];
                strncpy(destination, afterDash, l);
                destination[l] = '\0';
                cModule *source = parent->getSubmodule("app")->getSubmodule("a");
                cModule *dest = grandParent->getModuleByPath(destination)->getSubmodule("app")->getSubmodule("b");
                if (dest != NULL){
                    cGate *sourceGate = source->gate("out", 0);
                    cGate *destGate = dest->gate("in");
//                    std::cout << "connecting: " << sourceGate->getFullPath() << " to: " << destGate->getFullPath() << "\n";
                    cGate *prevGate = destGate->getPreviousGate();
                    if (prevGate != NULL){
                        prevGate->disconnect();
                    }
                    sourceGate->disconnect();
                    sourceGate->connectTo(destGate);
                }
            } else {
                // this plays 'b' role, receiver
                // now, receive from whom?
                // work backward from pch (<--|)
                while (pch[0] != '|'){
                    pch = pch - 1;
                }
                char * pch2 = strchr(pch, '|') + 1;
                char * pch3 = strchr(pch2, '-');
                int l = pch3 - pch2;
//                printf("Receiving from: %.*s\n", l, pch2);
            }
        }

        delete msgx;
        return;
    }
    scheduleAt(simTime()+par("sendIaTime").doubleValue(), sendMessageEvent);
    inet::Coord coord;

    try {
        inet::MassMobility *mob = check_and_cast<inet::MassMobility *>(mobility);
        coord = mob->getCurrentPosition();
    } catch( std::exception e ) {
        return;
    }

    std::string coordInfo = coord.info();
    double x = coord.x;
    double y = coord.y;

    cMessage *msg = new cMessage();
    ContextSync *payload = new ContextSync();
    payload->setDevice(device);
    payload->setX(x);
    payload->setY(y);

    msg->addObject(payload);

    send(msg, "sync");

//        EV_INFO << " _________ " << coordInfo;
}
