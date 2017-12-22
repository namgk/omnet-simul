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
        app: App {
            @display("p=158,45");
        }
        coordinator: LocalCoordinator {
            @display("p=46,45");
        }
    connections:
        action --> {  delay = 0ms; } --> coordinator.action;
        coordinator.sync --> {  delay = 0ms; } --> sync;
}
simple Component
{
    parameters:
        @signal[brecv](type="long");
//        @statistic[msgRecv](title="msg recv"; source="brecv"; record=vector,stats; interpolationmode=none);
     
    gates:
        input in;
        output out;
}
simple Source
{
    parameters:
        volatile double sendIaTime @unit(s);
        @display("i=block/source");
        @signal[asent](type="long");
        @signal[asentactual](type="long");
//        @statistic[msgSent](title="msg sent"; source="asent"; record=vector,stats; interpolationmode=none);
        
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
        volatile double sendIaTime @unit(s);
    gates:
        input action;
        output sync;
}
simple CentralizedCoordinator
{
    parameters:
        volatile double coordinationPeriod @unit(s);
//        @signal[asentall](type="long");
//        @signal[brecvall](type="long");
        @signal[abratioall](type="long");
//        @statistic[asent](title="msg sent"; source="asentall"; record=vector,stats; interpolationmode=none);
//        @statistic[brecv](title="msg recv"; source="brecvall"; record=vector,stats; interpolationmode=none);
        @statistic[abratio](title="send/recv"; source="abratioall"; record=vector,stats; interpolationmode=none);
        
    gates:
        input sync[];
        output action[];
}

module App
{
    submodules:
        a: Source {
            @display("p=32,65");
        }
        b: Component {
            @display("p=134,37");
        }
        c: Sink {
            @display("p=250,37");
        }
        d: Sink {
            @display("p=134,112");
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
        channel Channel extends ned.DelayChannel
        {
            delay = 10ms;
        }
    submodules:
        mobileHost[numHosts]: MobileHost {
            parameters:
                @display("p=96,241;r=,,#707070");
        }
        cloudHost[numCloudHosts]: CloudHost {
            parameters:
                @display("p=96,85;i=device/server_l");
        }
        fogHost[numFogHosts]: FogHost {
                @display("i=device/switch");
        }
        centralizedCoordinator: CentralizedCoordinator {
            @display("p=272,85");
        }
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
