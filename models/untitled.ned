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


module MobileHost
{
    parameters:
        string mobilityType = default("StationaryMobility");
        @networkNode();
        @display("i=device/pocketpc_s");
    submodules:
        mobility: <mobilityType> like IMobility;
}

module CloudHost
{
    parameters:
        string mobilityType = default("StationaryMobility");
        @networkNode();
    submodules:
        mobility: <mobilityType> like IMobility;
}

module FogHost
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
    submodules:
        host[numHosts]: MobileHost {
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
}
