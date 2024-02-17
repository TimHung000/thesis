cplusplus {{
#include <vector>
#include "SubTask.h"
typedef std::vector<int> intVector;
typedef std::vector<SubTask*> subTaskVector;
}}

class intVector
{
    @existingClass;
}

class subTaskVector
{
    @existingClass;
}


//
// Task message definition for queueing framework
//
packet Task
{
    int64_t taskId;
    simtime_t creationTime;
    simtime_t totalWaitingTime; 	    // total time spent on waiting in queues
    simtime_t totalProcessingTime;      // total time spent in processing
    simtime_t totalPropagationTime;     // total time spent in propagation
    simtime_t finishedTime;
   	simtime_t deadline;				 	// task need to finish before creattionTime + deadline
   	double taskSize;				    // bytes
   	double requiredCycle;
   	double processedCycle;
   	int arrivingServer;
   	int runningServer;
   	int destinationServer;
   	int hopCount;
   	bool isCompleted;
   	bool fromDispatcher;
   	intVector hopPath;
   	   	

   	// task can be divided into "totalSubTaskCount" subtasks, 
   	// the task size and required cpu cycle of each subtask are given
   	int totalSubTaskCount;
   	subTaskVector subTaskVec;          // all subtasks that this packet contain      
	
}