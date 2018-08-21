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
    aReusedMsg = 0;
    bReusedMsg = 0;
    cReusedMsg = 0;
    componentRecvMsg = 0;
    componentSentMsg = 0;
    droppedMsg = 0;
    solutionSentMsg = 0;
}

SignalListener::~SignalListener() {
}

void SignalListener::receiveSignal(cComponent *src, simsignal_t id, long d, cObject *details){
    const char sentSignal[] = "sent";
    const char recvSignal[] = "recv";
    const char aReused[] = "aReused";
    const char bReused[] = "bReused";
    const char cReused[] = "cReused";
    const char componentRecvSignal[] = "componentRecv";
    const char componentSentSignal[] = "componentSent";
    const char droppedSignal[] = "dropped";
    const char solutionSentSignal[] = "solutionsSent";
    const char *signalName = src->getSignalName(id);
    if (strcmp (signalName, sentSignal) == 0){
        sentMsg++;
    } else if (strcmp (signalName, recvSignal) == 0){
        recvMsg++;
    } else if (strcmp (signalName, aReused) == 0){
        aReusedMsg++;
    } else if (strcmp (signalName, bReused) == 0){
        bReusedMsg++;
    } else if (strcmp (signalName, cReused) == 0){
        cReusedMsg++;
    } else if (strcmp (signalName, componentSentSignal) == 0){
        componentSentMsg++;
    } else if (strcmp (signalName, componentRecvSignal) == 0){
        componentRecvMsg++;
    } else if (strcmp (signalName, droppedSignal) == 0){
        droppedMsg++;
    } else if (strcmp (signalName, solutionSentSignal) == 0){
        solutionSentMsg += d;
    }
}


