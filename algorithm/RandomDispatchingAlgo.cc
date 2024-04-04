/*
 * RandomDispatchingAlgo.cc
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#include "RandomDispatchingAlgo.h"
#include "TaskQueue.h"


RandomDispatchingAlgo::RandomDispatchingAlgo(SchedulingAlgo *schedulingAlgo, TaskQueue *taskQueue) {
    this->schedulingAlgo = schedulingAlgo;
    this->taskQueue = taskQueue;
}

RandomDispatchingAlgo::~RandomDispatchingAlgo() {}

void RandomDispatchingAlgo::execute(omnetpp::cMessage *msg) {
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed the deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        taskQueue->totalTaskFailed += 1;
        taskQueue->send(incomingTask, "taskFinishedOut");
        return;
    }

    omnetpp::simtime_t predictedFinishedTime = omnetpp::simTime() +
            (taskQueue->totalRequiredCycle - taskQueue->getCurrentRunningTaskFinishedCycle() + incomingTask->getRequiredCycle())
                / taskQueue->serverFrequency;

    if (taskQueue->serverMemory - taskQueue->totalMemoryConsumed >= incomingTask->getTaskSize() &&
            predictedFinishedTime <= incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        schedulingAlgo->insertTaskIntoWaitingQueue(incomingTask);
    } else {
        int randInt = taskQueue->intrand(taskQueue->neighborServers.size());
        incomingTask->setDestinationServer(taskQueue->neighborServers[randInt]);
        taskQueue->send(incomingTask, "offloadOut");
    }
    schedulingAlgo->scheduling();
}

