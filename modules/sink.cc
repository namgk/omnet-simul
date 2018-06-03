//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "inet/mobility/base/MobilityBase.h"
#include <math.h>

using namespace omnetpp;


/**
 * Packet sink; see NED file for more info.
 */
class Sink : public cSimpleModule
{
    cModule *parent;//app
    cModule *grandParent;//host
    cModule *mobility;
    int nearbyDef;

    private:
        simsignal_t recvSignal;
        simsignal_t droppedSignal;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Sink);

void Sink::initialize()
{
    parent = getParentModule();
    if (!parent){
        return;
    }
    grandParent = parent->getParentModule();
    if (!grandParent){
        return;
    }

    mobility = grandParent->getSubmodule("mobility");
    nearbyDef = grandParent->getParentModule()->par("nearbyDef").operator int();

    recvSignal = registerSignal("recv");
    droppedSignal = registerSignal("dropped");
}

void Sink::handleMessage(cMessage *msg)
{
    try {
        inet::MobilityBase *mob = check_and_cast<inet::MobilityBase *>(mobility);
        inet::Coord coord = mob->getCurrentPosition();
        double x = coord.x;
        double y = coord.y;
        double peerX = msg->par("x").doubleValue();
        double peerY = msg->par("y").doubleValue();
        if (std::abs (peerX - x) <= nearbyDef && std::abs (peerY - y) <= nearbyDef ){
            emit(recvSignal, 1);
        } else {
            emit(droppedSignal, 1);
        }
        EV_INFO << " RECEIVE MESSAGE, SOURCE COORDINATE:  X" << x << " Y: " << y;
    } catch( std::exception e ) {
        EV_ERROR << msg << endl;
    }
    delete msg;
}

