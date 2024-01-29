cplusplus {{
#include <vector>
#include <string>
typedef std::vector<std::string> StringVector;
}}

class StringVector
{
    @existingClass;
}
//
// Task message definition for queueing framework
//
packet Task
{
    simtime_t creationTime;
    simtime_t totalWaitingTime; 	    // total time spent on waiting in queues
    simtime_t totalProcessingTime;      // total time spent in processing
    simtime_t totalPropagationTime;     // total time spent in propagation
   	simtime_t deadline;				 // task need to be finisehd before this
   	double taskSize = 0;		
   	double cpuCycles = 1000000000;
   	double processedCycles = 0;		
   	int arrivingServer;
   	int runningServer;
   	StringVector hopPath;
}


