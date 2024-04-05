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
    omnetpp::simtime_t incomingTaskDeadline = incomingTask->getCreationTime() + incomingTask->getDelayTolerance();
    // task exceed the deadline
    if (omnetpp::simTime() > incomingTaskDeadline) {
        taskQueue->totalTaskFailed += 1;
        taskQueue->send(incomingTask, "taskFinishedOut");
        return;
    }

    if (incomingTask->getSubTaskVec().size() > 1) {
        splitAndOffload(incomingTask);
    } else {
        omnetpp::simtime_t predictedFinishedTime = omnetpp::simTime() +
                (taskQueue->totalRequiredCycle - taskQueue->getCurrentRunningTaskFinishedCycle() + incomingTask->getRequiredCycle())
                    / taskQueue->serverFrequency;
        if (taskQueue->serverMemory - taskQueue->totalMemoryConsumed >= incomingTask->getTaskSize() &&
                predictedFinishedTime <= incomingTaskDeadline) {
            schedulingAlgo->insertTaskIntoWaitingQueue(incomingTask);
        } else {
            taskDispatching(incomingTask);
        }
    }

    schedulingAlgo->scheduling();

}

void GreedyPartitionDispatchingAlgo::splitAndOffload(Task *task) {
    subTaskVector& subTaskVec = task->getSubTaskVecForUpdate();
    std::vector<Task*> taskToOffload;
    for (int i = 0; i < subTaskVec.size(); ++i)
        taskToOffload.push_back(createSubTask(task, i));
    taskQueue->cancelAndDelete(task);

    for (Task *curTask: taskToOffload) {
        omnetpp::simtime_t predictedFinishedTime = omnetpp::simTime() +
                (taskQueue->totalRequiredCycle - taskQueue->getCurrentRunningTaskFinishedCycle() + curTask->getRequiredCycle())
                    / taskQueue->serverFrequency;
        omnetpp::simtime_t curTaskDeadline = curTask->getCreationTime() + curTask->getDelayTolerance();
        if (taskQueue->serverMemory - taskQueue->totalMemoryConsumed >= curTask->getTaskSize() &&
                predictedFinishedTime <= curTaskDeadline) {
            schedulingAlgo->insertTaskIntoWaitingQueue(curTask);
        } else {
            taskDispatching(curTask);
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

    task->setDestinationServer(neighborServerStatusVec[0]->getServerId());
    taskQueue->send(task, "offloadOut");

    for (auto& status: neighborServerStatusVec)
        delete status;
}
