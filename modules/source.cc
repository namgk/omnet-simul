//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "inet/mobility/single/MassMobility.h"
#include <cmath>

using namespace omnetpp;

/**
 * Generates messages or jobs; see NED file for more info.
 */
class Source : public cSimpleModule
{
    const char *name;
    const char *device;

    private:
        cMessage *sendMessageEvent;
        simsignal_t sentSignal;
        simsignal_t sentSignalActual;

    public:
        Source();
        virtual ~Source();

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Source);

Source::Source()
{
    sendMessageEvent = nullptr;
}

Source::~Source()
{
    cancelAndDelete(sendMessageEvent);
}

void Source::initialize()
{
    cModule *parent = getParentModule();
    if (!parent){
        return;
    }
    cModule *grandParent = parent->getParentModule();
    if (!grandParent){
        return;
    }

    sentSignal = registerSignal("sent");

    name = getName();
    device = grandParent->getFullName();
    EV_INFO << " >>>> " << device << " >>>> " << name;

    sendMessageEvent = new cMessage("sendMessageEvent");
    scheduleAt(simTime(), sendMessageEvent);
}

void Source::handleMessage(cMessage *msg)
{
    ASSERT(msg == sendMessageEvent);

    const char hostName[] = "mobileHost";
    if (strcmp (hostName, getParentModule()->getParentModule()->getName()) != 0){
        return;
    }

    emit(sentSignal, 1);
    cGate *g = gate("out", 0);
    cGate *otherGate = g->getNextGate();
    if (otherGate){
        try {
            cModule *peerDevice = otherGate->getOwnerModule()->getParentModule()->getParentModule();

            if (peerDevice){
                inet::Coord coord;
                inet::Coord peerCoord;

                inet::MobilityBase *mobThis = check_and_cast<inet::MobilityBase *>(getParentModule()->getParentModule()->getSubmodule("mobility"));
                inet::MobilityBase *mob = check_and_cast<inet::MobilityBase *>(peerDevice->getSubmodule("mobility"));

                coord = mobThis->getCurrentPosition();
                peerCoord = mob->getCurrentPosition();

                double x = coord.x;
                double y = coord.y;
                double peerX = peerCoord.x;
                double peerY = peerCoord.y;

                if (std::abs (peerX - x) > 1000 || std::abs (peerY - y) > 1000 ){
//                    printf("---------------DISCONNECTING------------------- %lf %lf %lf %lf\n", x, y, peerX, peerY);
                    g->disconnect();
                } else {
                    cMessage *job = new cMessage("job");
                    send(job, "out", 0);
                }

            }
        } catch( std::exception e ) {
            printf("eeeeeeeeeeeeeeeeeeeeeeeee %s\n", e.what());
        }

    }

    scheduleAt(simTime()+par("sendIaTime").doubleValue(), sendMessageEvent);
}


