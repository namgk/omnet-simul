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
        long aReusedMsg;
        long bReusedMsg;
        long cReusedMsg;
        long componentRecvMsg;
        long componentSentMsg;
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

    long getComponentRecvMsg() const {
        return componentRecvMsg;
    }

    void setComponentRecvMsg(long componentRecvMsg) {
        this->componentRecvMsg = componentRecvMsg;
    }

    long getComponentSentMsg() const {
        return componentSentMsg;
    }

    void setComponentSentMsg(long componentSentMsg) {
        this->componentSentMsg = componentSentMsg;
    }

    long getAReusedMsg() const {
        return aReusedMsg;
    }

    void setAReusedMsg(long reusedMsg) {
        aReusedMsg = reusedMsg;
    }

    long getBReusedMsg() const {
        return bReusedMsg;
    }

    void setBReusedMsg(long reusedMsg) {
        bReusedMsg = reusedMsg;
    }

    long getCReusedMsg() const {
        return cReusedMsg;
    }

    void setCReusedMsg(long reusedMsg) {
        cReusedMsg = reusedMsg;
    }

    protected:
        virtual void receiveSignal(cComponent *src, simsignal_t id, long d, cObject *details);
};

