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
        @signal[componentSent](type="long");
        @signal[componentRecv](type="long");
        @statistic[componentSent](title="componentSent"; source="componentSent"; record=vector,stats; interpolationmode=none);
        @statistic[componentRecv](title="componentRecv"; source="componentRecv"; record=vector,stats; interpolationmode=none);
        
    gates:
        input in;
        output out;
}
simple Source
{
    parameters:
        volatile double sendIaTime @unit(s);
        @display("i=block/source");
        @signal[sent](type="long");
        @statistic[sent](title="send"; source="sent"; record=vector,stats; interpolationmode=none);

    gates:
        output out[2];
}
simple Sink
{
    parameters:
        @display("i=block/sink");
        @signal[recv](type="long");
        @signal[dropped](type="long");
        @statistic[recv](title="recv"; source="recv"; record=vector,stats; interpolationmode=none);
        @statistic[dropped](title="dropped"; source="dropped"; record=vector,stats; interpolationmode=none);
    gates:
        input in;
}
simple LocalCoordinator
{
    parameters:
        volatile double sendIaTime @unit(s);
        @signal[speed](type="double");
        @statistic[speed](title="speed"; source="speed"; record=vector,stats; interpolationmode=none);
        @signal[bFound](type="long");
        @statistic[bFound](title="bFound"; source="bFound"; record=vector,stats; interpolationmode=none);
        @signal[cFound](type="long");
        @statistic[cFound](title="cFound"; source="cFound"; record=vector,stats; interpolationmode=none);
        @signal[reuseConnection](type="long");
        @statistic[reuseConnection](title="reuseConnection"; source="reuseConnection"; record=vector,stats; interpolationmode=none);
        @signal[reuseConnectionBC](type="long");
        @statistic[reuseConnectionBC](title="reuseConnectionBC"; source="reuseConnectionBC"; record=vector,stats; interpolationmode=none);
    gates:
        input action;
        output sync;
}
simple CentralizedCoordinator
{
    parameters:
        volatile double coordinationPeriod @unit(s);
        @signal[solutionsSent](type="long");
        @signal[aReused](type="long");
        @signal[bReused](type="long");
        @signal[cReused](type="long");
        @statistic[solutionsSent](title="solutionsSent"; source="solutionsSent"; record=vector,stats; interpolationmode=none);
        @statistic[aReused](title="aReused"; source="aReused"; record=vector,stats; interpolationmode=none);
        @statistic[bReused](title="bReused"; source="bReused"; record=vector,stats; interpolationmode=none);
        @statistic[cReused](title="cReused"; source="cReused"; record=vector,stats; interpolationmode=none);

    gates:
        input sync[];
        output action[];
}

simple ResultCollector
{
    parameters:
        volatile double sendIaTime @unit(s);
	    volatile double coordinationPeriod @unit(s);
        @signal[sentrecv](type="long");
        @signal[bskipped](type="long");
        @signal[totalSolution](type="long");
        @signal[dropped](type="long");
        @signal[reused](type="long");
        @statistic[sentrecv](title="send/recv"; source="sentrecv"; record=vector,stats; interpolationmode=none);
        @statistic[totalSolution](title="totalSolution"; source="totalSolution"; record=vector,stats; interpolationmode=none);
        @statistic[dropped](title="dropped"; source="dropped"; record=vector,stats; interpolationmode=none);
        @statistic[bskipped](title="bskipped"; source="bskipped"; record=vector,stats; interpolationmode=none);
        @statistic[reused](title="reused"; source="reused"; record=vector,stats; interpolationmode=none);
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
        @display("i=device/pocketpc_s;bgb=252,117");
    submodules:
        mobility: <mobilityType> like IMobility {
            @display("p=215,86");
        }
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
        @display("bgb=252,117");
    submodules:
        mobility: <mobilityType> like IMobility;
}

network FogNetwork
{
    parameters:
        int numHosts;
        int nearbyDef;
        int solverTimeLimit;
        int numFogHosts;
        int numCloudHosts;
        int numCoord;
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
            @display("i=device/switch;p=289,240");
        }
        centralizedCoordinator[numCoord]: CentralizedCoordinator {
        }
        resultCollector: ResultCollector {
        }
    connections:
        for i=0..numHosts-1 {
            mobileHost[i].sync --> Channel --> centralizedCoordinator[i % numCoord].sync++;
        }
        for i=0..numHosts-1 {
            centralizedCoordinator[i % numCoord].action++ --> Channel --> mobileHost[i].action;
        }
        for i=0..numFogHosts-1 {
            fogHost[i].sync --> Channel --> centralizedCoordinator[i % numCoord].sync++;
        }
        for i=0..numFogHosts-1 {
            centralizedCoordinator[i % numCoord].action++ --> Channel --> fogHost[i].action;
        }
//        for i=0..numCloudHosts/2 {
//            cloudHost[i].sync --> Channel --> centralizedCoordinator.sync++;
//        }
//        for i=0..numCloudHosts/2 {
//            centralizedCoordinator.action++ --> Channel --> cloudHost[i].action;
//        }
        
        
//        for i=0..numHosts-1, if i%2 != 0 {
//            mobileHost[i].sync --> Channel --> centralizedCoordinator[1].sync++;
//        }
//        for i=0..numHosts-1, if i%2 != 0 {
//            centralizedCoordinator[1].action++ --> Channel --> mobileHost[i].action;
//        }
//        for i=0..numFogHosts-1, if i%2 != 0 {
//            fogHost[i].sync --> Channel --> centralizedCoordinator[1].sync++;
//        }
//        for i=0..numFogHosts-1, if i%2 != 0 {
//            centralizedCoordinator[1].action++ --> Channel --> fogHost[i].action;
//        }
//        for i=numCloudHosts/2+1..numCloudHosts-1 {
//            cloudHost[i].sync --> Channel --> centralizedCoordinator.sync++;
//        }
//        for i=numCloudHosts/2+1..numCloudHosts-1 {
//            centralizedCoordinator.action++ --> Channel --> cloudHost[i].action;
//        }
}
