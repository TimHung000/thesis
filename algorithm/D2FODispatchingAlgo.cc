/*
 * D2FODispatchingAlgo.cc
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */


#include <vector>
#include <algorithm>

#include "D2FODispatchingAlgo.h"
#include "TaskQueue.h"
#include "ServerStatus.h"


D2FODispatchingAlgo::D2FODispatchingAlgo(SchedulingAlgo *schedulingAlgo, TaskQueue *taskQueue) {
    this->schedulingAlgo = schedulingAlgo;
    this->taskQueue = taskQueue;
}

D2FODispatchingAlgo::~D2FODispatchingAlgo() {}

void D2FODispatchingAlgo::execute(omnetpp::cMessage *msg) {
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);
    omnetpp::simtime_t incomingTaskDeadline = incomingTask->getCreationTime() + incomingTask->getDelayTolerance();

    // task exceed the deadline
    if (omnetpp::simTime() > incomingTaskDeadline) {
        taskQueue->totalTaskFailed += 1;
        taskQueue->send(incomingTask, "taskFinishedOut");
        return;
    }

    omnetpp::simtime_t predictedFinishedTime = omnetpp::simTime() +
            (taskQueue->totalRequiredCycle - taskQueue->getCurrentRunningTaskFinishedCycle() + incomingTask->getRequiredCycle())
                / taskQueue->serverFrequency;

    if (taskQueue->serverMemory - taskQueue->totalMemoryConsumed >= incomingTask->getTaskSize() &&
            predictedFinishedTime <= incomingTaskDeadline)
        schedulingAlgo->insertTaskIntoWaitingQueue(incomingTask);
    else
        taskDispatching(incomingTask);

    schedulingAlgo->scheduling();
}

void D2FODispatchingAlgo::taskDispatching(Task *task) {
    std::vector<ServerStatus*> neighborServerStatusVec;

    for (int i = 0; i < taskQueue->neighborServers.size(); ++i)
        neighborServerStatusVec.push_back(taskQueue->getServerStatus(taskQueue->neighborServers[i]));

    std::sort(neighborServerStatusVec.begin(), neighborServerStatusVec.end(), [](ServerStatus *lhs, ServerStatus *rhs) {
        return lhs->getTotalRequiredCycle() / lhs->getServerFrequency() < rhs->getTotalRequiredCycle() / rhs->getServerFrequency();
    });

    task->setDestinationServer(neighborServerStatusVec[0]->getServerId());
    taskQueue->send(task, "offloadOut");

    for (auto& status: neighborServerStatusVec)
        delete status;
}
