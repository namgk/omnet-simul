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

package models;

import inet.mobility.group.MoBANCoordinator;
import inet.mobility.contract.IMobility;
import inet.node.internetcloud.InternetCloud;

module CoordinatedHost
{
    gates:
        input action;
        output sync;
	submodules:
	    app: App;
        coordinator: LocalCoordinator;
    connections:
		action --> {delay= 0ms;} --> coordinator.action;               
		coordinator.sync --> {delay= 0ms;} --> sync;        
}
simple Component
{
    gates:
        input in;
        output out;
}
simple Source
{
    parameters:
        volatile double sendIaTime @unit(s) = exponential(1s);
        @display("i=block/source");
    gates:
        output out[2];
}
simple Sink
{
    parameters:
        @display("i=block/sink");
    gates:
        input in;
}
simple LocalCoordinator
{
    parameters:
        volatile double sendIaTime @unit(s) = exponential(1s);
    gates:
        input action;
        output sync;
}
simple CentralizedCoordinator
{
    parameters:
        volatile double coordinationPeriod @unit(s) = exponential(2s);
    gates:
        input sync[];
        output action[];
}

module App
{
    submodules:
        a: Source {
            @display("p=163,6");
        }
        b: Component {
            @display("p=97,78");
        }
        c: Sink;
        d: Sink {
            @display("p=193,84");
        }
    connections allowunconnected:
}

module MobileHost extends CoordinatedHost
{
    parameters:
        string mobilityType = default("MassMobility");
        @networkNode();
        @display("i=device/pocketpc_s");
    submodules:
        mobility: <mobilityType> like IMobility;
}

module CloudHost extends CoordinatedHost
{
    parameters:
        string mobilityType = default("StationaryMobility");
        @networkNode();
    submodules:
        mobility: <mobilityType> like IMobility;
}

module FogHost extends CoordinatedHost
{
    parameters:
        string mobilityType = default("StationaryMobility");
        @networkNode();
    submodules:
        mobility: <mobilityType> like IMobility;
}

network FogNetwork
{
    parameters:
        int numHosts;
        int numFogHosts;
        int numCloudHosts;
    types:
        channel Channel extends ned.DelayChannel {
            delay = 10ms;
        }
    submodules:
        mobileHost[numHosts]: MobileHost {
            parameters:
                @display("p=353,273;r=,,#707070");
        }
        cloudHost[numCloudHosts]: CloudHost {
            parameters:
                @display("p=110,146;i=device/server_l");
        }
        fogHost[numFogHosts]: FogHost {
            parameters:
                @display("p=383,120;i=device/switch");
        }
        centralizedCoordinator: CentralizedCoordinator;
    connections:
        for i=0..numHosts-1 {
            mobileHost[i].sync --> Channel --> centralizedCoordinator.sync++;
        }
        for i=0..numHosts-1 {
            centralizedCoordinator.action++ --> Channel --> mobileHost[i].action;
        }
        for i=0..numFogHosts-1 {
            fogHost[i].sync --> Channel --> centralizedCoordinator.sync++;
        }
        for i=0..numFogHosts-1 {
            centralizedCoordinator.action++ --> Channel --> fogHost[i].action;
        }
        for i=0..numCloudHosts-1 {
            cloudHost[i].sync --> Channel --> centralizedCoordinator.sync++;
        }
        for i=0..numCloudHosts-1 {
            centralizedCoordinator.action++ --> Channel --> cloudHost[i].action;
        }
}
