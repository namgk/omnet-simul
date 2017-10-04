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

class MobileHost : public cModule
{
    const char *device;

    private:
        cMessage *sendMessageEvent;
    public:
        MobileHost();
        virtual ~MobileHost();
    protected:
        // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
};

//Define_Module(MobileHost);

MobileHost::MobileHost()
{
    sendMessageEvent = nullptr;
}

MobileHost::~MobileHost()
{
}

void MobileHost::initialize()
{
    device = getName();
    EV_INFO << " ____ " << device << " ____ ";

            cModule *mobility = getSubmodule("mobility");
    if (!mobility){
        return;
    }


    EV_INFO << " ____ " << device << " ____ " << mobility->getFullName();

    sendMessageEvent = new cMessage("sendMessageEvent");
}

