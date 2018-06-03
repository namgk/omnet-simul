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
    sentMsg = 0;
    recvMsg = 0;
    droppedMsg = 0;
    solutionSentMsg = 0;
}

SignalListener::~SignalListener() {
}

void SignalListener::receiveSignal(cComponent *src, simsignal_t id, long d, cObject *details){
    const char sentSignal[] = "sent";
    const char recvSignal[] = "recv";
    const char droppedSignal[] = "dropped";
    const char solutionSentSignal[] = "solutionsSent";
    const char *signalName = src->getSignalName(id);
    if (strcmp (signalName, sentSignal) == 0){
        sentMsg++;
    } else if (strcmp (signalName, recvSignal) == 0){
        recvMsg++;
    } else if (strcmp (signalName, droppedSignal) == 0){
        droppedMsg++;
    } else if (strcmp (signalName, solutionSentSignal) == 0){
        solutionSentMsg += d;
    }
}


