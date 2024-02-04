cplusplus {{
#include <vector>
typedef std::vector<int> intVector;
}}

class intVector
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
   	simtime_t deadline;				 	// task need to finish before creattionTime + deadline
   	double taskSize = 0;				// bytes
   	double cpuCycles = 1000000000;
   	double processedCycles = 0;
   	int arrivingServer = -1;
   	int runningServer = -1;
   	int destinationServer = -1;
   	int hopCount = 0;
   	bool isCompleted = false;
   	intVector hopPath;
}


