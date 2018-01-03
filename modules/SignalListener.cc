/*
 * SignalListener.cpp
 *
 *  Created on: Nov 27, 2017
 *      Author: nhong
 */

#include <omnetpp/clistener.h>
#include "SignalListener.h"

using namespace omnetpp;

SignalListener::SignalListener() {
    // TODO Auto-generated constructor stub
    sentMsg = 0;
    recvMsg = 0;
}

SignalListener::~SignalListener() {
    // TODO Auto-generated destructor stub
}

void SignalListener::receiveSignal(cComponent *src, simsignal_t id, long d, cObject *details){
    const char sentSignal[] = "sent";
    const char recvSignal[] = "recv";
    const char *signalName = src->getSignalName(id);
    if (strcmp (signalName, sentSignal) == 0){
        sentMsg++;
    } else if (strcmp (signalName, recvSignal) == 0){
        recvMsg++;
    }
}


