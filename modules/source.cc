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
 * Generates messages or jobs; see NED file for more info.
 */
class Source : public cSimpleModule
{
    const char *name;
    const char *device;

    private:
        cMessage *sendMessageEvent;

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

    name = getName();
    device = grandParent->getFullName();
    EV_INFO << " >>>> " << device << " >>>> " << name;

    sendMessageEvent = new cMessage("sendMessageEvent");
    scheduleAt(simTime(), sendMessageEvent);
}

void Source::handleMessage(cMessage *msg)
{
    ASSERT(msg == sendMessageEvent);

    for (int i = 0; i < gateSize("out"); i++) {
        cGate *g = gate("out", i);
        cGate *otherGate = g->getType()==cGate::OUTPUT ? g->getNextGate() : g->getPreviousGate();
        if (otherGate){
            cMessage *job = new cMessage("job");
            send(job, "out", i);
        } else {
        }
    }

    scheduleAt(simTime()+par("sendIaTime").doubleValue(), sendMessageEvent);
}


