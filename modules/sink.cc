//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

using namespace omnetpp;


/**
 * Packet sink; see NED file for more info.
 */
class Sink : public cSimpleModule
{
private:
    simsignal_t recvSignal;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Sink);

void Sink::initialize()
{
    recvSignal = registerSignal("recv");
}

void Sink::handleMessage(cMessage *msg)
{
    emit(recvSignal, 1);
    delete msg;
}

