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


module EdgeServer
{
    parameters:
        int address;
        @display("i=misc/node_vs,gold");
    gates:
        inout ports[];
        inout infoPorts[];
    submodules:
        source: Source {
            @display("p=60,135");
        }
        merge: Merge {
            @display("p=60,239");
            gates:
                in[sizeof(parent.ports)];
        }
        processor: Processor {
            
            @display("p=323,135");
        }
        taskCompletion: Sink {
            @display("p=507,135");
        }
        taskDrop: Sink {
            @display("p=234,248");
        }
        infoCollector: InfoCollector {
            @display("p=437,239");
            gates:
                otherEdgeServerInfoPorts[sizeof(parent.infoPorts)];
        }
        dispatcher: Dispatcher {
            @display("p=186,135");
            gates:
                otherEdgeServerOut[sizeof(parent.ports)];
        }
    connections:
        // random genearate tasks
        source.out --> IdealChannel --> dispatcher.taskIn++;
		
		// offloaded tasks from other edge server
        for i=0..sizeof(ports) - 1 {
            ports$i[i] --> IdealChannel --> merge.in[i];
        }
        merge.out --> IdealChannel --> dispatcher.taskIn++;
        
        // offload the task to this edger server or others
        dispatcher.myServerOut --> IdealChannel --> processor.in;
        for i=0..sizeof(ports) - 1 {
            dispatcher.otherEdgeServerOut[i] --> IdealChannel --> ports$o[i];
        }
        dispatcher.drop --> IdealChannel --> taskDrop.in++;

		
        
        processor.taskCompleteOut --> IdealChannel --> taskCompletion.in++;
        processor.taskDropOut --> IdealChannel --> taskDrop.in++;

		// collect info and distribute them
        processor.infoOut --> IdealChannel --> infoCollector.processorInfoIn;
        for i=0..sizeof(infoPorts) - 1 {
            infoPorts[i] <--> IdealChannel <--> infoCollector.otherEdgeServerInfoPorts[i];
        }
        infoCollector.infoOut --> IdealChannel --> dispatcher.infoIn;

}
