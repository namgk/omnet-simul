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
        long asent;
        long asentactual;
        long brecv;
    public:
        SignalListener();
        virtual ~SignalListener();

    long getAsent() const {
        return asent;
    }

    void setAsent(long asent) {
        this->asent = asent;
    }

    long getBrecv() const {
        return brecv;
    }

    void setBrecv(long brecv) {
        this->brecv = brecv;
    }

    long getAsentactual() const {
        return asentactual;
    }

    void setAsentactual(long asentactual) {
        this->asentactual = asentactual;
    }

    protected:
        virtual void receiveSignal(cComponent *src, simsignal_t id, long d, cObject *details);
};

