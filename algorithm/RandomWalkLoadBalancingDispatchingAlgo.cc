/*
 * RandomWalkLoadBalancingDispatchingAlgo.cc
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#include "RandomWalkLoadBalancingDispatchingAlgo.h"
#include "TaskQueue.h"


RandomWalkLoadBalancingDispatchingAlgo::RandomWalkLoadBalancingDispatchingAlgo(SchedulingAlgo *schedulingAlgo, TaskQueue *taskQueue)
    : maximumHop(taskQueue->par("maxHop")), threshold(taskQueue->par("threshold")) {
    this->schedulingAlgo = schedulingAlgo;
    this->taskQueue = taskQueue;
}

RandomWalkLoadBalancingDispatchingAlgo::~RandomWalkLoadBalancingDispatchingAlgo() {}

void RandomWalkLoadBalancingDispatchingAlgo::execute(omnetpp::cMessage *msg) {
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);
    omnetpp::simtime_t incomingTaskDeadline = incomingTask->getCreationTime() + incomingTask->getDelayTolerance();
    // task exceed the deadline
    if (omnetpp::simTime() > incomingTaskDeadline) {
        taskQueue->totalTaskFailed += 1;
        taskQueue->send(incomingTask, "taskFinishedOut");
        return;
    }

    double remainedUnprocessedCycle = taskQueue->totalRequiredCycle - taskQueue->getCurrentRunningTaskFinishedCycle();
    double systemLoad = (remainedUnprocessedCycle / taskQueue->serverFrequency)
                                / (incomingTaskDeadline - omnetpp::simTime()).dbl();
    int steps = incomingTask->getHopCount();

    if (steps >= maximumHop) {
        if (taskQueue->serverMemory - taskQueue->totalMemoryConsumed >= incomingTask->getTaskSize())
            schedulingAlgo->insertTaskIntoWaitingQueue(incomingTask);
        else
            taskQueue->garbageQueue.push_back(incomingTask);
    } else {
//        double curThreshold = static_cast<double>(steps) / maximumHop * threshold;
        if (systemLoad <= threshold) {
            if (taskQueue->serverMemory - taskQueue->totalMemoryConsumed >= incomingTask->getTaskSize())
                schedulingAlgo->insertTaskIntoWaitingQueue(incomingTask);
            else
                taskQueue->garbageQueue.push_back(incomingTask);
        } else {
            int randInt = taskQueue->intrand(taskQueue->neighborServers.size());
            incomingTask->setDestinationServer(taskQueue->neighborServers[randInt]);
            taskQueue->send(incomingTask, "offloadOut");
        }
    }
    schedulingAlgo->scheduling();
}
