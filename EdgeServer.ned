import ned.IdealChannel;


module EdgeServer
{
    parameters:
        int id;
        int numUsers;
        @display("i=misc/node_vs,gold");
    gates:
        inout ports[];
        inout infoPorts[];
        output taskFinishedOut;
    submodules:
        users[numUsers]: Source {
            @display("p=60,135");
        }
        processor: Processor {
            @display("p=403,135");
        }
        infoHandler: InfoHandler {
            @display("p=437,239");
            gates:
                otherEdgeServerInfoPorts[sizeof(parent.infoPorts)];
        }
        dispatcher: Dispatcher {
            @display("p=251,135");
        }
        finishedTaskCollector: Merge {
            @display("p=290,260");
        }
        routing: Routing {
            @display("p=138,82");
            gates:
                in[sizeof(parent.ports)];
                out[sizeof(parent.ports)];
        }
    connections:
        // random genearate tasks
        for i=0..sizeof(users) - 1 {
            users[i].out --> IdealChannel --> routing.localIn++;
        }

        // task destination is this server, so dispatcher need to decide where task should go
        routing.localOut --> IdealChannel --> dispatcher.taskIn++;

        for i=0..sizeof(ports) - 1 {
            // other server offload task to this server
            ports$i[i] --> IdealChannel --> routing.in[i];
            // offload task to other server
            routing.out[i] --> IdealChannel --> ports$o[i];
        }

        dispatcher.offloadOut --> IdealChannel --> routing.localIn++;
        dispatcher.localProcessOut --> IdealChannel --> processor.in;
        dispatcher.taskFinishedOut --> IdealChannel --> finishedTaskCollector.in++;

        processor.infoOut --> IdealChannel --> infoHandler.processorInfoIn;
        for i=0..sizeof(infoPorts) - 1 {
            infoPorts[i] <--> IdealChannel <--> infoHandler.otherEdgeServerInfoPorts[i];
        }

        // completed and drop task collection
        processor.taskFinishedOut --> IdealChannel --> finishedTaskCollector.in++;
        finishedTaskCollector.out --> IdealChannel --> taskFinishedOut;
}
