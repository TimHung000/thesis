import ned.IdealChannel;


module EdgeServer
{
    parameters:
        @display("i=misc/node_vs,gold");
    gates:
        inout ports[];
        output taskFinishedOut;
        input userTaskIn;
    submodules:
        taskQueue: TaskQueue {
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
    	
    	userTaskIn --> IdealChannel --> routing.localIn++;

        // task destination is this server, so dispatcher need to decide where task should go
        routing.localOut --> IdealChannel --> taskQueue.taskIn++;

        for i=0..sizeof(ports) - 1 {
            // other server offload task to this server
            ports$i[i] --> IdealChannel --> routing.in[i];
            // offload task to other server
            routing.out[i] --> IdealChannel --> ports$o[i];
        }

        taskQueue.offloadOut --> IdealChannel --> routing.localIn++;
        taskQueue.taskFinishedOut --> IdealChannel --> finishedTaskCollector.in++;

        // completed and drop task collection
        finishedTaskCollector.out --> IdealChannel --> taskFinishedOut;
}
