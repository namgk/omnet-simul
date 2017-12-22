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
    asent = 0;
    asentactual = 0;
    brecv = 0;
}

SignalListener::~SignalListener() {
    // TODO Auto-generated destructor stub
}

void SignalListener::receiveSignal(cComponent *src, simsignal_t id, long d, cObject *details){
    const char aSentSignal[] = "asent";
    const char aSentActualSignal[] = "asentactual";
    const char bRecvSignal[] = "brecv";
    const char *signalName = src->getSignalName(id);
    if (strcmp (signalName, aSentSignal) == 0){
        asent++;
    } else if (strcmp (signalName, aSentActualSignal) == 0){
        asentactual++;
    } else if (strcmp (signalName, bRecvSignal) == 0){
        brecv++;
    }
}


