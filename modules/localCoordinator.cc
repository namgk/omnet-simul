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
        // Coordination action got from centralized coordinator
        std::string actionsStr(msgx->par("action").stringValue());

        std::stringstream ss(actionsStr);
        std::istream_iterator<std::string> begin(ss);
        std::istream_iterator<std::string> end;
        std::vector<std::string> vstrings(begin, end);

        std::vector<std::string>::iterator itr;
        for ( itr = vstrings.begin(); itr < vstrings.end(); ++itr ){
            // am I involved?
            // FIXME: 'me' could present in two different location. E.g a,b,c => 1,2,3; 2,3,1; 3,1,2
            const char* actions = itr->c_str();
            char *me = strstr(actions, device);
            if (me == NULL){
                // I'm not involved, do nothing
                continue;
            }

            // e.g actions: |mobileHost[2]-cloudHost[0]-mobileHost[5]|mobileHost[3]-cloudHost[1]-mobileHost[1]
            // e.g device: mobileHost[3]
            // looking for my position
            if (me == actions){
                // this plays 'a' role, sender, e.g mobileHost[3] or mobileHost[2]
                // now, send to whom? e.g cloudHost[1] --> calculate destination

                // finding the next '-' pointer, starting after it to get the actual name!
                char *afterDash = strchr(me, '-') + 1;
                // finding the coming '-' pointer
                char *nextDash = strchr(afterDash, '-');

                // extract destination
                int length = nextDash - afterDash;
                char destination[length+1];
                strncpy(destination, afterDash, length);
                destination[length] = '\0';

//                printf("a: %.*s\n", 30, me);
//                printf("b: %s\n  ", destination);

                // look up destination address, assuming address table is shared with all participant
                cModule *aNode = parent->getSubmodule("app")->getSubmodule("a");
                cModule *bNode = grandParent->getModuleByPath(destination)->getSubmodule("app")->getSubmodule("b");

                if (bNode != NULL){
                    cGate *aGate = aNode->gate("out", 0);
                    cGate *bGate = bNode->gate("in");

                    // disconnect b first so that this a can connect to it
                    cGate *bPrevGate = bGate->getPreviousGate();
                    if (bPrevGate != NULL && bPrevGate == aGate){
                        continue;
                    }

                    if (bPrevGate != NULL){
                        bPrevGate->disconnect();
                    }

                    aGate->disconnect();
                    aGate->connectTo(bGate);
                }
            } else {
                // this plays 'b' or 'c' role, |mobileHost[2]-cloudHost[0]-mobileHost[5]|mobileHost[4]-
                // determining if it's b, we need to find it's destination too, similar to the a case

                // finding the next '-' pointer
                char *nextDash = strchr(me, '-');
                if (nextDash != NULL){
                    // this plays b
                    char *end = strchr(nextDash, '\0');

                    // extract destination
                    int length = end - nextDash - 1;
                    char destination[length+1];
                    strncpy(destination, nextDash + 1, length);
                    destination[length] = '\0';

//                    printf("b: %.*s\n", 25, me);
//                    printf("c: %s\n  ", destination);

                    // look up destination address, assuming address table is shared with all participant
                    cModule *bNode = parent->getSubmodule("app")->getSubmodule("b");
                    cModule *cNode = grandParent->getModuleByPath(destination)->getSubmodule("app")->getSubmodule("c");

                    if (cNode != NULL){
                        cGate *bGate = bNode->gate("out");
                        cGate *cGatee = cNode->gate("in");

                        // disconnect c first so that this b can connect to it
                        cGate *cPrevGate = cGatee->getPreviousGate();
                        if (cPrevGate != NULL && cPrevGate == bGate){
                            continue;
                        }

                        if (cPrevGate != NULL){
                            cPrevGate->disconnect();
                        }

                        bGate->disconnect();
                        bGate->connectTo(cGatee);
                    }
                } else {
                    // this plays c, pass!
                }
            }
        }

        delete msgx;
        return;
    }

    // periodic context synchronization with centralized coordinator
    scheduleAt(simTime()+par("sendIaTime").doubleValue(), sendMessageEvent);
    inet::Coord coord;

    try {
        inet::MassMobility *mob = check_and_cast<inet::MassMobility *>(mobility);
        coord = mob->getCurrentPosition();
    } catch( std::exception e ) {
        // skip context sync for stationary components because
        // assuming we only use location as the only dynamic context data here
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
