/*
 * ProposedDispatchingAlgo.cc
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#include <vector>
#include <algorithm>
#include <queue>
#include <list>

#include "ProposedDispatchingAlgo.h"
#include "ServerStatus.h"
#include "Task_m.h"
#include "TaskQueue.h"
#include "Utility.h"


ProposedDispatchingAlgo::ProposedDispatchingAlgo(SchedulingAlgo *schedulingAlgo, TaskQueue *taskQueue) {
    this->schedulingAlgo = schedulingAlgo;
    this->taskQueue = taskQueue;
}

ProposedDispatchingAlgo::~ProposedDispatchingAlgo() {}

void ProposedDispatchingAlgo::execute(omnetpp::cMessage *msg) {
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed the deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        taskQueue->totalTaskFailed += 1;
        taskQueue->send(incomingTask, "taskFinishedOut");
        return;
    }

    if (incomingTask->getSubTaskVec().size() == 1) { // task cannot split
        taskOffloading(incomingTask);
    } else {  // task can split
        bool successSplitAndOffload = trySplitAndOffload(incomingTask);
        if (!successSplitAndOffload)
            taskDispatching(incomingTask);
//            taskOffloading(incomingTask);
    }
    schedulingAlgo->scheduling();
}

bool ProposedDispatchingAlgo::trySplitAndOffload(Task *task) {
    omnetpp::simtime_t taskDeadline = task->getCreationTime() + task->getDelayTolerance();
    subTaskVector& subTaskVec = task->getSubTaskVecForUpdate();

    std::vector<ServerStatus*> serverStatusVec;
    for (int i = 0; i < taskQueue->neighborServers.size(); ++i)
        serverStatusVec.push_back(taskQueue->getServerStatus(taskQueue->neighborServers[i]));
    serverStatusVec.push_back(taskQueue->getServerStatus());

    std::sort(serverStatusVec.begin(), serverStatusVec.end(), [](ServerStatus *lhs,ServerStatus *rhs) {
        return lhs->getTotalRequiredCycle() / lhs->getServerFrequency() < rhs->getTotalRequiredCycle() / rhs->getServerFrequency();
    });

    std::sort(subTaskVec.begin(), subTaskVec.end(), [](SubTask *lhs, SubTask *rhs) {
        return lhs->getSubTaskRequiredCPUCycle() > rhs->getSubTaskRequiredCPUCycle();
    });

    const double predictedTimeInTransmit = taskQueue->getSystemModule()->par("linkDelay").doubleValue();

    SubTask *curSubTask;
    int serverIdx = 0;
    int subTaskIdx = 0;
    while (subTaskIdx < subTaskVec.size() && serverIdx < serverStatusVec.size()) {
        curSubTask = subTaskVec[subTaskIdx];
        omnetpp::simtime_t predictedFinishedTime = (serverStatusVec[serverIdx]->getTotalRequiredCycle() + curSubTask->getSubTaskRequiredCPUCycle())
                / serverStatusVec[serverIdx]->getServerFrequency() + omnetpp::simTime();
        predictedFinishedTime += (serverStatusVec[serverIdx]->getServerId() == taskQueue->serverId) ? 0 : predictedTimeInTransmit;
        if (predictedFinishedTime > taskDeadline)
            break;

        ++subTaskIdx;
        ++serverIdx;
    }
    bool canSplitTask = subTaskIdx == subTaskVec.size();

    if (canSplitTask) {
        std::vector<Task*> taskToOffload;
        for (int i = 0; i < subTaskVec.size(); ++i)
            taskToOffload.push_back(createSubTask(task, i));
        taskQueue->cancelAndDelete(task);

        serverIdx = 0;
        for (Task *curTask: taskToOffload) {
            if (serverStatusVec[serverIdx]->getServerId() == taskQueue->serverId) {
                schedulingAlgo->insertTaskIntoWaitingQueue(curTask);
            } else {
                curTask->setDestinationServer(serverStatusVec[serverIdx]->getServerId());
                taskQueue->send(curTask, "offloadOut");
            }
            ++serverIdx;
        }
    }
    return canSplitTask;
}

void ProposedDispatchingAlgo::taskOffloading(Task *task) {
    if (task->getSubTaskVec().size() == task->getTotalSubTaskCount()) { // task is whole task
        if (isNeighborLoadLessThanServerLoad()) {
            taskDispatching(task);
        } else {
            std::vector<Task*> taskToOffload = preemptive(task);
            for (Task *task: taskToOffload)
                taskDispatching(task);
        }
    } else {  // task is not whole task
        std::vector<Task*> taskToOffload = preemptive(task);
        for (Task *task: taskToOffload)
            taskDispatching(task);
    }
}

bool ProposedDispatchingAlgo::isNeighborLoadLessThanServerLoad() {
    ServerStatus *thisServerStatus = taskQueue->getServerStatus();
    std::vector<ServerStatus*> neighborServerStatusVec;
    for (int i = 0; i < taskQueue->neighborServers.size(); ++i)
        neighborServerStatusVec.push_back(taskQueue->getServerStatus(taskQueue->neighborServers[i]));

    double neighborTotalRequiredCycle = 0.0;
    double neighborTotalFrequency = 0.0;
    for (auto curServerStatus: neighborServerStatusVec) {
        neighborTotalRequiredCycle += curServerStatus->getTotalRequiredCycle();
        neighborTotalFrequency += curServerStatus->getServerFrequency();
    }
    const double predictedTimeInTransmit = taskQueue->getSystemModule()->par("linkDelay").doubleValue();

    omnetpp::simtime_t neighborAvgFinishedTime = omnetpp::simTime() + predictedTimeInTransmit + neighborTotalRequiredCycle / neighborTotalFrequency;

    bool res = false;
    if (neighborAvgFinishedTime <
            omnetpp::simTime() + thisServerStatus->getTotalRequiredCycle() / thisServerStatus->getServerFrequency())
        res = true;

    return res;
}

std::vector<Task*> ProposedDispatchingAlgo::preemptive(Task *task) {
    omnetpp::simtime_t taskDeadline = task->getCreationTime() + task->getDelayTolerance();

    // calculate max spare time of waiting queue and incoming task
    double maxSpareTime = taskDeadline.dbl() - task->getRequiredCycle() / taskQueue->serverFrequency - omnetpp::simTime().dbl();
    double curSpareTime;
    for (std::list<Task*>::iterator it = taskQueue->waitingQueue.begin(); it != taskQueue->waitingQueue.end(); ++it) {
        curSpareTime = ((*it)->getCreationTime() + (*it)->getDelayTolerance() - (*it)->getRequiredCycle() / taskQueue->serverFrequency - omnetpp::simTime()).dbl();
        if (curSpareTime > maxSpareTime)
            maxSpareTime = curSpareTime;
    }

    // priority queue for all score higher than incoming task, and it's place in waiting queue is before incoming task
    auto compare = [=](std::list<Task*>::iterator lhs, std::list<Task*>::iterator rhs) {
        double lhsSpareTime = ((*lhs)->getCreationTime() + (*lhs)->getDelayTolerance() - (*lhs)->getRequiredCycle() / taskQueue->serverFrequency - omnetpp::simTime()).dbl();
        double lhsWholeTask = static_cast<double>((*lhs)->getSubTaskVec().size() == (*lhs)->getTotalSubTaskCount());
        double lhsScore = getScore(lhsSpareTime, maxSpareTime, lhsWholeTask);
        double rhsSpareTime = ((*rhs)->getCreationTime() + (*rhs)->getDelayTolerance() - (*rhs)->getRequiredCycle() / taskQueue->serverFrequency - omnetpp::simTime()).dbl();
        double rhsWholeTask = static_cast<double>((*rhs)->getSubTaskVec().size() == (*rhs)->getTotalSubTaskCount());
        double rhsScore = getScore(rhsSpareTime, maxSpareTime, rhsWholeTask);
        return lhsScore < rhsScore;
    };
    std::priority_queue<std::list<Task*>::iterator, std::vector<std::list<Task*>::iterator>, decltype(compare)> maxHeap(compare);

    // build the priority queue, and find the place that incoming task should be inserted
    double accuRequiredCycle = (taskQueue->runningTask != nullptr) ? taskQueue->runningTask->getRequiredCycle() - taskQueue->getCurrentRunningTaskFinishedCycle() : 0.0;
    double canReleaseCycle = 0.0;
    double taskSpareTime = taskDeadline.dbl() - task->getRequiredCycle() / taskQueue->serverFrequency - omnetpp::simTime().dbl();
    double taskWholeTask = static_cast<double>(task->getSubTaskVec().size() == task->getTotalSubTaskCount());
    double taskScore = getScore(taskSpareTime, maxSpareTime, taskWholeTask);
    double curWholeTask, curScore;
    omnetpp::simtime_t curDeadline;
    std::list<Task*>::iterator it = taskQueue->waitingQueue.begin();
    while (it != taskQueue->waitingQueue.end()) {
        curSpareTime = ((*it)->getCreationTime() + (*it)->getDelayTolerance() - (*it)->getRequiredCycle() / taskQueue->serverFrequency - omnetpp::simTime()).dbl();
        curWholeTask = static_cast<double>((*it)->getSubTaskVec().size() == (*it)->getTotalSubTaskCount());
        curScore = getScore(curSpareTime, maxSpareTime, curWholeTask);

        if (curDeadline > taskDeadline)
            break;

        if (curScore > taskScore) {
            maxHeap.push(it);
            canReleaseCycle += (*it)->getRequiredCycle();
        }
        accuRequiredCycle += (*it)->getRequiredCycle();
        ++it;
    }


    // if incoming task can be placed in the waitingQueue,
    // put all the task need to move out to the taskItrToOffload
    std::vector<std::list<Task*>::iterator> taskItrToOffload;
    std::vector<Task*> taskToOffload;
    if (omnetpp::simTime() + (accuRequiredCycle - canReleaseCycle + task->getRequiredCycle()) / taskQueue->serverFrequency > taskDeadline)
        taskToOffload.push_back(task);
    else {
        std::list<Task*>::iterator tmpIt;
        while (!maxHeap.empty() &&
                omnetpp::simTime() + (accuRequiredCycle + task->getRequiredCycle()) / taskQueue->serverFrequency > taskDeadline) {
            tmpIt = maxHeap.top();
            maxHeap.pop();
            accuRequiredCycle -= (*tmpIt)->getRequiredCycle();
            taskItrToOffload.push_back(tmpIt);
        }

        taskQueue->waitingQueue.insert(it, task);
        accuRequiredCycle += task->getRequiredCycle();
        taskQueue->totalRequiredCycle += task->getRequiredCycle();
        taskQueue->totalMemoryConsumed += task->getTaskSize();

        // when the incoming task be inserted into this server, it will impact all the task after it.
        // move all task wouldn't finished in time to taskItrToOffload
        while (it != taskQueue->waitingQueue.end()) {
            if (omnetpp::simTime() + (accuRequiredCycle + (*it)->getRequiredCycle()) / taskQueue->serverFrequency > (*it)->getCreationTime() + (*it)->getDelayTolerance())
                taskItrToOffload.push_back(it);
            else
                accuRequiredCycle += (*it)->getRequiredCycle();
            ++it;
        }

        Task *curTask;
        for (int i = 0; i < taskItrToOffload.size(); ++i) {
            curTask = *taskItrToOffload[i];
            taskQueue->waitingQueue.erase(taskItrToOffload[i]);
            taskToOffload.push_back(curTask);
            taskQueue->totalRequiredCycle -= curTask->getRequiredCycle();
            taskQueue->totalMemoryConsumed -= curTask->getTaskSize();
        }
    } // end if incoming task can be inserted into waiting Queue

    return taskToOffload;
}

void ProposedDispatchingAlgo::taskDispatching(Task *task) {
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
