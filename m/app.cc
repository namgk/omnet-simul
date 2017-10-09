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

class App : public cModule
{
    const char *name;
    const char *device;

    private:
        cMessage *sendMessageEvent;
    public:
        App();
        virtual ~App();
    protected:
        // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
};

//Define_Module(App);

App::App()
{
    sendMessageEvent = nullptr;
}

App::~App()
{
}

void App::initialize()
{
    name = getName();
    EV_INFO << " ____ " << name << " ____ ";

            cModule *parent = getParentModule();
    if (!parent){
        return;
    }
    cModule *grandParent = parent->getParentModule();
    if (!grandParent){
        return;
    }

    device = parent->getFullName();
    sendMessageEvent = new cMessage("sendMessageEvent");
}

