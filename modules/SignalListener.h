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
        long droppedMsg;
        long solutionSentMsg;
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

    long getDroppedMsg() const {
        return droppedMsg;
    }

    void setDroppedMsg(long droppedMsg) {
        this->droppedMsg = droppedMsg;
    }

    long getSolutionSentMsg() const {
        return solutionSentMsg;
    }

    void setSolutionSentMsg(long solutionSentMsg) {
        this->solutionSentMsg = solutionSentMsg;
    }

    protected:
        virtual void receiveSignal(cComponent *src, simsignal_t id, long d, cObject *details);
};

