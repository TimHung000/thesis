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
    this->predictedLoadMultiple = 1.5;
    this->subTaskThreshold = 0.8;
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

    subTaskVector& subTaskVec = incomingTask->getSubTaskVecForUpdate();
    bool subTaskSizeOverThreshold = false;
    for (SubTask *subTask: subTaskVec) {
        if (subTask->getSubTaskSize() / incomingTask->getTaskSize() >= subTaskThreshold)
            subTaskSizeOverThreshold = true;
    }

    if (subTaskVec.size() == 1 || subTaskSizeOverThreshold) { // task cannot split
        taskOffloading(incomingTask);
    } else {  // task can split
        trySplitAndOffload(incomingTask);
    }
    schedulingAlgo->scheduling();
}

void ProposedDispatchingAlgo::trySplitAndOffload(Task *task) {
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


    omnetpp::simtime_t taskDeadline = task->getCreationTime() + task->getDelayTolerance();
    const double predictedTimeInTransmit = taskQueue->getSystemModule()->par("linkDelay").doubleValue();

    SubTask *curSubTask;
    int serverIdx = 0;
    int subTaskIdx = 0;
    while (subTaskIdx < subTaskVec.size() && serverIdx < serverStatusVec.size()) {
        curSubTask = subTaskVec[subTaskIdx];
        double curServerLoad = predictedLoadMultiple * (serverStatusVec[serverIdx]->getTotalRequiredCycle() / serverStatusVec[serverIdx]->getServerFrequency());
        double transmisionTime = (serverStatusVec[serverIdx]->getServerId() == taskQueue->serverId) ? 0 : predictedTimeInTransmit;
        omnetpp::simtime_t predictedFinishedTime = omnetpp::simTime() + curServerLoad + transmisionTime +
                curSubTask->getSubTaskRequiredCPUCycle() / serverStatusVec[serverIdx]->getServerFrequency();

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
        for (int i = 0; i < taskToOffload.size(); ++i) {
            taskToOffload[i]->setDestinationServer(serverStatusVec[i]->getServerId());
            taskQueue->send(taskToOffload[i], "offloadOut");
            ++serverIdx;
        }
    } else {
        taskDispatching(task);
    }
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
    double maxWholeRequiredCycle = task->getWholeRequiredCycle();
    double curWholeRequiredCycle;
    for (std::list<Task*>::iterator it = taskQueue->waitingQueue.begin(); it != taskQueue->waitingQueue.end(); ++it) {
        curSpareTime = ((*it)->getCreationTime() + (*it)->getDelayTolerance() - (*it)->getRequiredCycle() / taskQueue->serverFrequency - omnetpp::simTime()).dbl();
        curWholeRequiredCycle = (*it)->getWholeRequiredCycle();
        if (curSpareTime > maxSpareTime)
            maxSpareTime = curSpareTime;

        if (curWholeRequiredCycle > maxWholeRequiredCycle)
            maxWholeRequiredCycle = curWholeRequiredCycle;
    }

    // find all the task that can be moved out
    std::vector<Task*> taskToBeReplaced;
    double canReleaseCycle = 0.0;
    double taskScore = getScore(
                (task->getCreationTime() + task->getDelayTolerance() -
                        task->getRequiredCycle() / taskQueue->serverFrequency - omnetpp::simTime()).dbl(),
                maxSpareTime,
                task->getWholeRequiredCycle(),
                maxWholeRequiredCycle,
                (task->getSubTaskVec().size() == task->getTotalSubTaskCount()) ? 1.0 : 1.0 / static_cast<double>(task->getTotalSubTaskCount())
            );


    for (std::list<Task*>::iterator it = taskQueue->waitingQueue.begin(); it != taskQueue->waitingQueue.end(); ++it) {
        double curScore = getScore(
                    ((*it)->getCreationTime() + (*it)->getDelayTolerance() -
                        (*it)->getRequiredCycle() / taskQueue->serverFrequency - omnetpp::simTime()).dbl(),
                    maxSpareTime,
                    (*it)->getWholeRequiredCycle(),
                    maxWholeRequiredCycle,
                    ((*it)->getSubTaskVec().size() == (*it)->getTotalSubTaskCount()) ? 1.0 : 1.0 / static_cast<double>((*it)->getTotalSubTaskCount())
                );

        if (curScore > taskScore) {
            taskToBeReplaced.push_back(*it);
            canReleaseCycle += (*it)->getRequiredCycle();
        }
    }

    std::sort(taskToBeReplaced.begin(), taskToBeReplaced.end(), [=](const Task *lhs, const Task *rhs) {
        double lhsScore = getScore(
                (lhs->getCreationTime() + lhs->getDelayTolerance() -
                        lhs->getRequiredCycle() / taskQueue->serverFrequency - omnetpp::simTime()).dbl(),
                maxSpareTime,
                lhs->getWholeRequiredCycle(),
                maxWholeRequiredCycle,
                (lhs->getSubTaskVec().size() == lhs->getTotalSubTaskCount()) ? 1.0 : 1.0 / static_cast<double>(lhs->getTotalSubTaskCount())
            );
        double rhsScore = getScore(
                (rhs->getCreationTime() + rhs->getDelayTolerance() -
                        rhs->getRequiredCycle() / taskQueue->serverFrequency - omnetpp::simTime()).dbl(),
                maxSpareTime,
                rhs->getWholeRequiredCycle(),
                maxWholeRequiredCycle,
                (rhs->getSubTaskVec().size() == rhs->getTotalSubTaskCount()) ? 1.0 : 1.0 / static_cast<double>(rhs->getTotalSubTaskCount())
            );
        return lhsScore < rhsScore;
    });

    double remainedRequiredCycle = taskQueue->totalRequiredCycle - taskQueue->getCurrentRunningTaskFinishedCycle();
    std::vector<Task*> taskToOffload;
    if (omnetpp::simTime() + (remainedRequiredCycle - canReleaseCycle + task->getRequiredCycle()) / taskQueue->serverFrequency > taskDeadline)
        taskToOffload.push_back(task);
    else {
        for (std::vector<Task*>::reverse_iterator rit = taskToBeReplaced.rbegin(); rit != taskToBeReplaced.rend(); ++rit) {
            Task *curTask = *rit;
            if (omnetpp::simTime() + (remainedRequiredCycle + task->getRequiredCycle()) / taskQueue->serverFrequency <= taskDeadline)
                break;
            taskQueue->waitingQueue.erase(std::find(taskQueue->waitingQueue.begin(), taskQueue->waitingQueue.end(), curTask));
            taskQueue->totalRequiredCycle -= curTask->getRequiredCycle();
            taskQueue->totalMemoryConsumed -= curTask->getTaskSize();
            taskToOffload.push_back(curTask);
            remainedRequiredCycle -= curTask->getRequiredCycle();
        }
        schedulingAlgo->insertTaskIntoWaitingQueue(task);

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

    for (auto& status: neighborServerStatusVec)
        delete status;
}
