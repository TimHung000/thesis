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
import ned.IdealChannel;


network EdgeEnv
{
    parameters:
    	int n = 5
    	@display("bgb=348,251");
    submodules:
    	edgeServer[n]: EdgeServer {
    		address = index + 1;
    	}
//        edgeServer1: EdgeServer {
//            address = 1;
//            @display("p=88,36");
//        }
//        edgeServer2: EdgeServer {
//            address = 2;
//            @display("p=247,36");
//        }
//        edgeServer3: EdgeServer {
//            @display("p=45,140");
//        }
//        edgeServer4: EdgeServer {
//            @display("p=162,199");
//        }
//        edgeServer5: EdgeServer {
//            @display("p=292,140");
//        }
    connections:
        for i = 0..n-1, for j = i+1..n-1 {
            edgeServer[i].ports++ <--> IdealChannel <--> edgeServer[j].ports++;
            edgeServer[i].infoPorts++ <--> IdealChannel <--> edgeServer[j].infoPorts++;
        }
//        edgeServer1.ports++ <--> IdealChannel <--> edgeServer2.ports++;
//        edgeServer1.infoPorts++ <--> IdealChannel <--> edgeServer2.infoPorts++;
//        edgeServer1.port++ <--> IdealChannel <--> edgeServer3.port++;
//        edgeServer1.port++ <--> IdealChannel <--> edgeServer4.port++;
//        edgeServer1.port++ <--> IdealChannel <--> edgeServer5.port++;
//        edgeServer2.port++ <--> IdealChannel <--> edgeServer3.port++;
//        edgeServer2.port++ <--> IdealChannel <--> edgeServer4.port++;
//        edgeServer2.port++ <--> IdealChannel <--> edgeServer5.port++;
//        edgeServer3.port++ <--> IdealChannel <--> edgeServer4.port++;
//        edgeServer3.port++ <--> IdealChannel <--> edgeServer5.port++;
//        edgeServer4.port++ <--> IdealChannel <--> edgeServer5.port++;

}
