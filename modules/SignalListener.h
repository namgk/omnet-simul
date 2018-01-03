/*
 * SignalListener.h
 *
 *  Created on: Nov 8, 2017
 *      Author: nhong
 */

#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class SignalListener: public cListener {
    public:
        long sentMsg;
        long recvMsg;
    public:
        SignalListener();
        virtual ~SignalListener();

    long getSent() const {
        return sentMsg;
    }

    void setSent(long sent) {
        this->sentMsg = sent;
    }

    long getRecv() const {
        return recvMsg;
    }

    void setRecv(long recv) {
        this->recvMsg = recv;
    }

    protected:
        virtual void receiveSignal(cComponent *src, simsignal_t id, long d, cObject *details);
};

