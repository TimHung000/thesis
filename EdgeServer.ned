import ned.IdealChannel;


module EdgeServer
{
    parameters:
        int id;
        int numUsers = default(1);
        @display("i=misc/node_vs,gold");
    gates:
        inout ports[];
        inout infoPorts[];
        output taskFinishedOut;
    submodules:
        users[numUsers]: Source {
            @display("p=60,135");
        }
        otherServer: Merge {
            @display("p=60,239");
            gates:
                in[sizeof(parent.ports)];
        }
        processor: Processor {
            @display("p=323,135");
        }
        infoHandler: InfoHandler {
            @display("p=437,239");
            gates:
                otherEdgeServerInfoPorts[sizeof(parent.infoPorts)];
        }
        dispatcher: Dispatcher {
            @display("p=186,135");
            gates:
                otherEdgeServerOut[sizeof(parent.ports)];
        }
        finishedTaskCollector: Merge {
            @display("p=290,260");
        }
    connections:
        // random genearate tasks
        for i=0..sizeof(users) - 1 {
            users[i].out --> IdealChannel --> dispatcher.taskIn++;
        }

		// offloaded tasks from other edge server
        for i=0..sizeof(ports) - 1 {
            ports$i[i] --> IdealChannel --> otherServer.in[i];
        }
        otherServer.out --> IdealChannel --> dispatcher.taskIn++;

        // offload the task to this edger server or others
        dispatcher.myServerOut --> IdealChannel --> processor.in;
        for i=0..sizeof(ports) - 1 {
            dispatcher.otherEdgeServerOut[i] --> IdealChannel --> ports$o[i];
        }
        
        
        
        processor.infoOut --> IdealChannel --> infoHandler.processorInfoIn;
        for i=0..sizeof(infoPorts) - 1 {
            infoPorts[i] <--> IdealChannel <--> infoHandler.otherEdgeServerInfoPorts[i];
        }
        
        // completed and drop task collection
        processor.taskFinishedOut --> IdealChannel --> finishedTaskCollector.in++;
        dispatcher.taskFinishedOut --> IdealChannel --> finishedTaskCollector.in++;
        finishedTaskCollector.out --> IdealChannel --> taskFinishedOut;
}
