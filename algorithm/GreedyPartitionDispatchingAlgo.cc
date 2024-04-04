/*
 * GreedyPartitionDispatchingAlgo.cc
 *
 *  Created on: Apr 4, 2024
 *      Author: tim
 */


#include "GreedyPartitionDispatchingAlgo.h"

#include <vector>
#include <algorithm>

#include "ServerStatus.h"
#include "Task_m.h"
#include "TaskQueue.h"
#include "Utility.h"

GreedyPartitionDispatchingAlgo::GreedyPartitionDispatchingAlgo(SchedulingAlgo *schedulingAlgo, TaskQueue *taskQueue) {
    this->schedulingAlgo = schedulingAlgo;
    this->taskQueue = taskQueue;
}

GreedyPartitionDispatchingAlgo::~GreedyPartitionDispatchingAlgo() {}

void GreedyPartitionDispatchingAlgo::execute(omnetpp::cMessage *msg) {
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    if (incomingTask->getSubTaskVec().size() > 1) {
        splitAndOffload(incomingTask);
    } else {
        omnetpp::simtime_t predictedFinishedTime = omnetpp::simTime() +
                (taskQueue->totalRequiredCycle - taskQueue->getCurrentRunningTaskFinishedCycle() + incomingTask->getRequiredCycle())
                    / taskQueue->serverFrequency;
        if (taskQueue->serverMemory - taskQueue->totalMemoryConsumed >= incomingTask->getTaskSize() &&
                predictedFinishedTime <= incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
            schedulingAlgo->insertTaskIntoWaitingQueue(incomingTask);
        } else {
            taskDispatching(incomingTask);
        }
    }
}

void GreedyPartitionDispatchingAlgo::splitAndOffload(Task *task) {
    subTaskVector& subTaskVec = task->getSubTaskVecForUpdate();
    std::vector<Task*> taskToOffload;
    for (int i = 0; i < subTaskVec.size(); ++i)
        taskToOffload.push_back(createSubTask(task, i));
    taskQueue->cancelAndDelete(task);

    std::vector<ServerStatus*> serverStatusVec;
    for (int i = 0; i < taskQueue->neighborServers.size(); ++i)
        serverStatusVec.push_back(taskQueue->getServerStatus(taskQueue->neighborServers[i]));
    serverStatusVec.push_back(taskQueue->getServerStatus());

    std::sort(serverStatusVec.begin(), serverStatusVec.end(), [](ServerStatus *lhs,ServerStatus *rhs) {
        return lhs->getTotalRequiredCycle() / lhs->getServerFrequency() < rhs->getTotalRequiredCycle() / rhs->getServerFrequency();
    });

    std::sort(taskToOffload.begin(), taskToOffload.end(), [](Task *lhs, Task *rhs) {
        return lhs->getRequiredCycle() > rhs->getRequiredCycle();
    });

    for (int i = 0; i < taskToOffload.size(); ++i) {
        if (serverStatusVec[i]->getServerId() == taskQueue->serverId) {
            schedulingAlgo->insertTaskIntoWaitingQueue(taskToOffload[i]);
        } else {
            taskToOffload[i]->setDestinationServer(serverStatusVec[i]->getServerId());
            taskQueue->send(taskToOffload[i], "offloadOut");
        }
    }
}

void GreedyPartitionDispatchingAlgo::taskDispatching(Task *task) {
    std::vector<ServerStatus*> neighborServerStatusVec;

    for (int i = 0; i < taskQueue->neighborServers.size(); ++i)
        neighborServerStatusVec.push_back(taskQueue->getServerStatus(taskQueue->neighborServers[i]));

    std::sort(neighborServerStatusVec.begin(), neighborServerStatusVec.end(), [](ServerStatus *lhs, ServerStatus *rhs) {
        return lhs->getTotalRequiredCycle() / lhs->getServerFrequency() < rhs->getTotalRequiredCycle() / rhs->getServerFrequency();
    });

    int i = 0;
    while (task->getHopPath().size() >= 2 &&
            neighborServerStatusVec[i]->getServerId() == task->getHopPath()[task->getHopPath().size()-2])
        ++i;

    task->setDestinationServer(neighborServerStatusVec[i]->getServerId());
    taskQueue->send(task, "offloadOut");
}
