//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Component : public cSimpleModule
{
  const char *name;
  const char *device;

  private:
    simsignal_t recvSignal;

  protected:
    // The following redefined virtual function holds the algorithm.
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Component);

void Component::initialize()
{
    cModule *parent = getParentModule();
    if (!parent){
        return;
    }
    cModule *grandParent = parent->getParentModule();
    if (!grandParent){
        return;
    }

    recvSignal = registerSignal("brecv");

    name = getName();
    device = grandParent->getFullName();
    EV_INFO << " >>>> " << device << " >>>> " << name;
}

void Component::handleMessage(cMessage *msg)
{
    emit(recvSignal, 1);
    cGate *g = gate("out");
    cGate *otherGate = g->getType()==cGate::OUTPUT ? g->getNextGate() : g->getPreviousGate();
    if (otherGate){
        send(msg, "out");
    } else {
        delete msg;
    }
}
