/*
 * LocalDispatchingAlgo.cc
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#include "LocalDispatchingAlgo.h"
#include "TaskQueue.h"

LocalDispatchingAlgo::LocalDispatchingAlgo(SchedulingAlgo *schedulingAlgo, TaskQueue *taskQueue) {
    this->schedulingAlgo = schedulingAlgo;
    this->taskQueue = taskQueue;
}

LocalDispatchingAlgo::~LocalDispatchingAlgo() {}

void LocalDispatchingAlgo::execute(omnetpp::cMessage *msg) {
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed the deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        taskQueue->totalTaskFailed += 1;
        taskQueue->send(incomingTask, "taskFinishedOut");
        return;
    }

    if (taskQueue->serverMemory - taskQueue->totalMemoryConsumed >= incomingTask->getTaskSize()) {
        schedulingAlgo->insertTaskIntoWaitingQueue(incomingTask);
    } else {
        taskQueue->totalTaskFailed += 1;
        taskQueue->send(incomingTask, "taskFinishedOut");
    }
    schedulingAlgo->scheduling();

}
