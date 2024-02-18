/*
 * Dispatcher.cc
 *
 *  Created on: Jan 29, 2024
 *      Author: tim
 */

#include "TaskQueue.h"

#include <vector>
#include <random>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "Task_m.h"
#include "SubTask.h"
#include <queue>
#include <limits>

Define_Module(TaskQueue);

TaskQueue::~TaskQueue()
{
    for (std::list<Task*>::iterator it = FIFOwaitingQueue.begin(); it != FIFOwaitingQueue.end(); ++it)
        cancelAndDelete(*it);

    for (std::list<Task*>::iterator it = wholeTaskWaitingQueue.begin(); it != wholeTaskWaitingQueue.end(); ++it)
        cancelAndDelete(*it);

    for (std::list<Task*>::iterator it = subTaskWaitingQueue.begin(); it != subTaskWaitingQueue.end(); ++it)
        cancelAndDelete(*it);

    for (std::list<Task*>::iterator it = garbageQueue.begin(); it != garbageQueue.end(); ++it)
        cancelAndDelete(*it);

    if (runningTask)
        cancelAndDelete(runningTask);
}

void TaskQueue::initialize()
{
    // server basic info
    serverId = getParentModule()->getIndex();
    int outputGateSize = getParentModule()->gateSize("ports$o");
    for (int i = 0; i < outputGateSize; ++i){
        int neighborServerIndex = getParentModule()->gate("ports$o", i)->getNextGate()->getOwnerModule()->getIndex();
        neighborSevers.push_back(neighborServerIndex);
    }

    // initialize server capacity
    double randFrequency = par("minFrequency").doubleValue() +
            intrand(par("maxFrequency").doubleValue() - par("minFrequency").doubleValue() + 1);
    serverFrequency = static_cast<double>(randFrequency) * 1e9;
    serverMemory = serverFrequency * par("memoryMultiple").doubleValue();

    // initialize server status
    totalRequiredCycle = 0;
    totalMemoryConsumed = 0;

    // for dispatching algorithm
    dispatchingAlgo = par("dispatchingAlgo").stdstringValue();


    // for scheduling algorithm
    schedulingAlgo = par("schedulingAlgo").stdstringValue();


    // for random algo used
    memoryThreshold = par("memoryThreshold");
    maximumHop = par("maximumHop");

    // for processing task management
    runningTask = nullptr;


    // signal
    memoryLoadingSignal = registerSignal("memoryLoading");
    CPULoadingSignal = registerSignal("CPULoading");
    totalTaskFinishedSignal = registerSignal("totalTaskFinished");
    totalTaskCompletedSignal = registerSignal("totalTaskCompleted");

}

void TaskQueue::handleMessage(omnetpp::cMessage *msg)
{
    if (msg == runningTask) {
        ASSERT(runningTask != nullptr);
        omnetpp::simtime_t processingTime = omnetpp::simTime() - runningTask->getSendingTime();
        double processedCycle = processingTime.dbl() * serverFrequency;
        runningTask->setTotalProcessingTime(runningTask->getTotalProcessingTime() + processingTime);
        runningTask->setProcessedCycle(runningTask->getProcessedCycle() + processedCycle);
        runningTask->setFinishedTime(omnetpp::simTime());
        runningTask->setIsCompleted(true);
        totalRequiredCycle -= runningTask->getRequiredCycle();
        totalMemoryConsumed -= runningTask->getTaskSize();
        send(runningTask, "taskFinishedOut");
        runningTask = nullptr;
        scheduling();
        return;
    }

    if (dispatchingAlgo == "Random") {
        randomDispatchingAlgo(msg);
    } else if (dispatchingAlgo == "Greedy") {
        greedyDispatchingAlgo(msg);
    } else if (dispatchingAlgo == "Proposed") {
        proposedDispatchingAlgo(msg);
    }

    scheduling();

}

void TaskQueue::randomDispatchingAlgo(omnetpp::cMessage *msg) {
    /***********************************************
     * Random Walk based Load Balancing Algorithm  *
     ***********************************************/

    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed the deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        intVector& hopPathForUpdate = incomingTask->getHopPathForUpdate();
        hopPathForUpdate.push_back(serverId);
        send(incomingTask, "taskFinishedOut");
        return;
    }

    // if the current server can accommodate the task then send to the processor
    // otherwise, randomly offload to neighbor server
    omnetpp::simtime_t predictedFinishedTime;
    if (runningTask) {
        predictedFinishedTime = runningTask->getArrivalTime() +
                (totalRequiredCycle - runningTask->getRequiredCycle() + incomingTask->getRequiredCycle()) / serverFrequency;
    } else {
        predictedFinishedTime = omnetpp::simTime() + (totalRequiredCycle + incomingTask->getRequiredCycle()) / serverFrequency;
    }
    if (serverMemory - totalMemoryConsumed >= incomingTask->getTaskSize() &&
            predictedFinishedTime <= incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        FIFOwaitingQueue.push_back(incomingTask);
    } else {
        int randInt = intrand(neighborSevers.size());
        incomingTask->setDestinationServer(neighborSevers[randInt]);
        send(incomingTask, "offloadOut");
    }
}

void TaskQueue::greedyDispatchingAlgo(omnetpp::cMessage *msg) {
    /***********************************************
     * Distributed Dynamic Offloading Mechanism    *
     ***********************************************/
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed the deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        intVector& hopPathForUpdate = incomingTask->getHopPathForUpdate();
        hopPathForUpdate.push_back(serverId);
        send(incomingTask, "taskFinishedOut");
        return;
    }

    // if the current server can accommodate the task, then send to the processor
    omnetpp::simtime_t predictedFinishedTime;
    if (runningTask) {
        predictedFinishedTime = runningTask->getArrivalTime() +
                (totalRequiredCycle - runningTask->getRequiredCycle() + incomingTask->getRequiredCycle()) / serverFrequency;
    } else {
        predictedFinishedTime = omnetpp::simTime() + (totalRequiredCycle + incomingTask->getRequiredCycle()) / serverFrequency;
    }

    if (serverMemory - totalMemoryConsumed >= incomingTask->getTaskSize() &&
            predictedFinishedTime <= incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        FIFOwaitingQueue.push_back(incomingTask);
    } else {

        // get neighbor server status
        std::vector<ServerStatus*> neighborServerStatus;
        int outputGateSize = getParentModule()->gateSize("ports$o");
        TaskQueue *neighborTaskQueueModule;
        ServerStatus *curNeighborStatus;
        for (int i = 0; i < outputGateSize; ++i){
            neighborTaskQueueModule  = omnetpp::check_and_cast<TaskQueue*>(getParentModule()->gate("ports$o", i)->getNextGate()->getOwnerModule()->getSubmodule("TaskQueue"));
            curNeighborStatus = neighborTaskQueueModule->getServerStatus();
            neighborServerStatus.push_back(curNeighborStatus);
        }

        std::sort(neighborServerStatus.begin(), neighborServerStatus.end(), [=](ServerStatus *lhs, ServerStatus *rhs) {
            double lhsPredictedDelay = (lhs->getTotalRequiredCycle() + incomingTask->getRequiredCycle()) / lhs->getServerFrequency();
            double rhsPredictedDelay = (rhs->getTotalRequiredCycle() + incomingTask->getRequiredCycle()) / rhs->getServerFrequency();

            return lhsPredictedDelay < rhsPredictedDelay;
        });

        int bestNeighborServerId = 0;
        for (int i = 0; i < neighborServerStatus.size(); ++i) {
            if (neighborServerStatus[i]->getTotalMemoryConsumed() + incomingTask->getTaskSize() <= neighborServerStatus[i]->getServerMemory()) {
                bestNeighborServerId = i;
                break;
            }
        }

        incomingTask->setDestinationServer(neighborServerStatus[bestNeighborServerId]->getServerId());
        send(incomingTask, "offloadOut");

        for (auto& status: neighborServerStatus)
            delete status;
    }
}

void TaskQueue::proposedDispatchingAlgo(omnetpp::cMessage *msg) {
    /***********************************************
     *           Proposed Method                   *
     ***********************************************/
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        intVector& hopPathForUpdate = incomingTask->getHopPathForUpdate();
        hopPathForUpdate.push_back(serverId);
        send(incomingTask, "taskFinishedOut");
        return;
    }

    omnetpp::simtime_t incomingTaskDeadline = incomingTask->getCreationTime() + incomingTask->getDelayTolerance();
    double incomingTaskProcessingTime = incomingTask->getRequiredCycle() / serverFrequency;
    double incomingTaskMatrix = (incomingTaskDeadline - incomingTaskProcessingTime).dbl();
    bool isIncomingTaskWholeTask = incomingTask->getTotalSubTaskCount() == incomingTask->getSubTaskVec().size();

    omnetpp::simtime_t incomingTaskFinishedTime;
    if (runningTask) {
        incomingTaskFinishedTime = runningTask->getArrivalTime() +
                (totalRequiredCycle - runningTask->getRequiredCycle() + incomingTask->getRequiredCycle()) / serverFrequency;
    } else {
        incomingTaskFinishedTime = omnetpp::simTime() + (totalRequiredCycle + incomingTask->getRequiredCycle()) / serverFrequency;
    }

    // incoming task can be processed by this server without any extra handling
    if (serverMemory - totalMemoryConsumed >= incomingTask->getTaskSize() && incomingTaskFinishedTime <= incomingTaskDeadline) {
        // whole task
        if (isIncomingTaskWholeTask) {

            std::list<Task*>::reverse_iterator rit = wholeTaskWaitingQueue.rbegin();
            while (rit != wholeTaskWaitingQueue.rend() &&
                    ((*rit)->getCreationTime() + (*rit)->getDelayTolerance()) -
                    ((*rit)->getRequiredCycle() / serverFrequency) > incomingTaskMatrix)
                ++rit;
            wholeTaskWaitingQueue.insert(rit.base(), incomingTask);
        } else { // sub task
            std::list<Task*>::reverse_iterator rit = subTaskWaitingQueue.rbegin();
            while (rit != subTaskWaitingQueue.rend() &&
                    ((*rit)->getCreationTime() + (*rit)->getDelayTolerance()) -
                    ((*rit)->getRequiredCycle() / serverFrequency) > incomingTaskMatrix)
                ++rit;
            subTaskWaitingQueue.insert(rit.base(), incomingTask);
        }

    } else { // the incoming task can not be put into this server, need extra handling


        // get server status
        ServerStatus *thisServerStatus = getServerStatus();

        std::vector<ServerStatus*> neighborServerStatus;
        int outputGateSize = getParentModule()->gateSize("ports$o");
        TaskQueue *neighborTaskQueueModule;
        ServerStatus *curNeighborStatus;
        for (int i = 0; i < outputGateSize; ++i){
            neighborTaskQueueModule  = omnetpp::check_and_cast<TaskQueue*>(getParentModule()->gate("ports$o", i)->getNextGate()->getOwnerModule()->getSubmodule("TaskQueue"));
            curNeighborStatus = neighborTaskQueueModule->getServerStatus();
            neighborServerStatus.push_back(curNeighborStatus);
        }


        // for preemption, which will replace the task in queue, and offload it out
        std::vector<Task*> dispatchingWholeTasks;

        // if incoming task is sub task. just try insert into it and remove the whole task to outside
        if (!isIncomingTaskWholeTask) {
            Task *curTask;
            while (!wholeTaskWaitingQueue.empty()) {
                curTask = wholeTaskWaitingQueue.back();
                wholeTaskWaitingQueue.pop_back();
                dispatchingWholeTasks.push_back(curTask);
                incomingTaskFinishedTime -= curTask->getRequiredCycle() / serverFrequency;

                if (incomingTaskFinishedTime <= incomingTaskDeadline)
                    break;
            }

            std::list<Task*>::reverse_iterator rit = subTaskWaitingQueue.rbegin();
            while (rit != subTaskWaitingQueue.rend() &&
                    ((*rit)->getCreationTime() + (*rit)->getDelayTolerance()) -
                    ((*rit)->getRequiredCycle() / serverFrequency) > incomingTaskMatrix)
                ++rit;

            subTaskWaitingQueue.insert(rit.base(), incomingTask);
        }

        // incoming task is whole task
        if (incomingTask->getTotalSubTaskCount() == incomingTask->getSubTaskVec().size())
            dispatchingWholeTasks.push_back(incomingTask);

        for (Task* task: dispatchingWholeTasks) {

            // if most of neighbor can processed the task, then split the task, otherwise, just offload the whole task
            double neighborAvailability = 0.0;
            omnetpp::simtime_t neighborServerPredictedFinishedTime;
            for (int i = 0; i < neighborServerStatus.size(); ++i) {
                neighborServerPredictedFinishedTime = omnetpp::simTime() + 5 * 1e-3 + (neighborServerStatus[i]->getTotalRequiredCycle() + task->getRequiredCycle())
                        / neighborServerStatus[i]->getServerFrequency();
                if (neighborServerPredictedFinishedTime <= task->getCreationTime() + task->getDelayTolerance())
                    neighborAvailability += 1;
            }
            neighborAvailability = neighborAvailability / static_cast<double>(neighborServerStatus.size());

            // less than half of neighbor can process the task, so just offload the whole task
            if (neighborAvailability < 0.5 || task->getSubTaskVec().size() == 1) {

                std::sort(neighborServerStatus.begin(), neighborServerStatus.end(), [=](ServerStatus *lhs, ServerStatus *rhs) {
                    double lhsPredictedDelay = (lhs->getTotalRequiredCycle() + task->getRequiredCycle()) / lhs->getServerFrequency();
                    double rhsPredictedDelay = (rhs->getTotalRequiredCycle() + task->getRequiredCycle()) / rhs->getServerFrequency();

                    return lhsPredictedDelay < rhsPredictedDelay;
                });

                std::vector<int> hopPath = task->getHopPath();
                int idx = 0;
                while (idx < neighborServerStatus.size()) {
                    if (hopPath.size() == 0)
                        break;

                    if (hopPath.size() == 1 &&
                                hopPath[hopPath.size()-1] != neighborServerStatus[idx]->getServerId())
                            break;

                    if (hopPath.size() >= 2 &&
                            hopPath[hopPath.size()-1] != neighborServerStatus[idx]->getServerId() &&
                            hopPath[hopPath.size()-2] != neighborServerStatus[idx]->getServerId())
                        break;
                    idx++;
                }
                if (idx == neighborServerStatus.size())
                    idx = 0;

                task->setDestinationServer(neighborServerStatus[idx]->getServerId());
                neighborServerStatus[idx]->setTotalMemoryConsumed(neighborServerStatus[idx]->getTotalMemoryConsumed() + task->getTaskSize());
                neighborServerStatus[idx]->setTotalRequiredCycle(neighborServerStatus[idx]->getTotalRequiredCycle() + task->getRequiredCycle());
                send(task, "offloadOut");
            } else { // more than half of neighbor can process the task, so split it and offload

                auto compare = [=](ServerStatus* lhs, ServerStatus* rhs) {
                    double lhsPredictedDelay = (lhs->getTotalRequiredCycle() + task->getRequiredCycle()) / lhs->getServerFrequency();
                    double rhsPredictedDelay = (rhs->getTotalRequiredCycle() + task->getRequiredCycle()) / rhs->getServerFrequency();
                    return lhsPredictedDelay > rhsPredictedDelay;
                };

                std::priority_queue<ServerStatus*, std::vector<ServerStatus*>, decltype(compare)> serverStatusHeap(compare);
                serverStatusHeap.push(thisServerStatus);
                for (ServerStatus* status: neighborServerStatus)
                    serverStatusHeap.push(status);


                subTaskVector& subTaskVec = task->getSubTaskVecForUpdate();
                // task has multiple subtasks, so sort it by its required cpu cycle. large..small
                std::sort(subTaskVec.begin(), subTaskVec.end(), [](const SubTask *lhs, const SubTask *rhs) {
                    return lhs->getSubTaskSize() >= rhs->getSubTaskSize();
                });

                int curSubTaskId = 0;
                ServerStatus *bestServerStatus;
                while (curSubTaskId < subTaskVec.size()) {
                    bestServerStatus = serverStatusHeap.top();
                    serverStatusHeap.pop();

                    Task *dupTask = task->dup();

                    dupTask->setTaskSize(subTaskVec[curSubTaskId]->getSubTaskSize());

                    dupTask->setRequiredCycle(subTaskVec[curSubTaskId]->getSubTaskRequiredCPUCycle());

                    std::vector<int> hopPath(task->getHopPath());
                    dupTask->setHopPath(hopPath);
                    dupTask->setSubTaskVec({subTaskVec[curSubTaskId]});
                    dupTask->setDestinationServer(bestServerStatus->getServerId());
                    send(dupTask, "offloadOut");

                    bestServerStatus->setTotalMemoryConsumed(bestServerStatus->getTotalMemoryConsumed() + subTaskVec[curSubTaskId]->getSubTaskSize());
                    bestServerStatus->setTotalRequiredCycle(bestServerStatus->getTotalRequiredCycle() + subTaskVec[curSubTaskId]->getSubTaskRequiredCPUCycle());

                    serverStatusHeap.push(bestServerStatus);

                    curSubTaskId += 1;
                }

                cancelAndDelete(task);

            }

        }
        delete thisServerStatus;
        for (ServerStatus *serverStatus: neighborServerStatus)
            delete serverStatus;
    }

}

void TaskQueue::scheduling() {
    if (runningTask)
        return;

    if (schedulingAlgo == "FIFO") {
        FIFOSchedulingAlgo();
    } else if (schedulingAlgo == "Proposed") {
        proposedSchedulingAlgo();
    }
}

void TaskQueue::FIFOSchedulingAlgo() {
    // remove all task that exceed deadline
    std::list<Task*>::iterator it = FIFOwaitingQueue.begin();
    while (it != FIFOwaitingQueue.end()) {
        if (omnetpp::simTime() > (*it)->getCreationTime() + (*it)->getDelayTolerance()) {
            totalRequiredCycle -= (*it)->getRequiredCycle();
            totalMemoryConsumed -= (*it)->getTaskSize();
            send((*it), "taskFinishedOut");
            it = FIFOwaitingQueue.erase(it);
        } else
            ++it;
    }

    // remove all task that exceed deadline
    it = garbageQueue.begin();
    while (it != garbageQueue.end()) {
        if (omnetpp::simTime() > (*it)->getCreationTime() + (*it)->getDelayTolerance()) {
            totalRequiredCycle -= (*it)->getRequiredCycle();
            totalMemoryConsumed -= (*it)->getTaskSize();
            send((*it), "taskFinishedOut");
            it = garbageQueue.erase(it);
        } else
            ++it;
    }

    // move tasks that will exceed the deadline to garbage queue
    it = FIFOwaitingQueue.begin();
    while (it != FIFOwaitingQueue.end() &&
            omnetpp::simTime() + (*it)->getRequiredCycle() / serverFrequency > (*it)->getCreationTime() + (*it)->getDelayTolerance()) {
        garbageQueue.push_back(*it);
        ++it;
        FIFOwaitingQueue.pop_front();
    }

    // assing the first task than can finished before deadline to the taskRunning
    if (it != FIFOwaitingQueue.end()) {
        FIFOwaitingQueue.pop_front();
        runningTask = *it;
        double runningTime = runningTask->getRequiredCycle() / serverFrequency;
        intVector& hopPathForUpdate = runningTask->getHopPathForUpdate();
        hopPathForUpdate.push_back(serverId);
        runningTask->setRunningServer(serverId);
        scheduleAt(omnetpp::simTime() + runningTime, runningTask);
    }
}

void TaskQueue::proposedSchedulingAlgo() {

    // remove all task that exceed deadline
    std::list<Task*>::iterator it = garbageQueue.begin();
    while (it != garbageQueue.end()) {
        if (omnetpp::simTime() > (*it)->getCreationTime() + (*it)->getDelayTolerance()) {
            totalRequiredCycle -= (*it)->getRequiredCycle();
            totalMemoryConsumed -= (*it)->getTaskSize();
            send((*it), "taskFinishedOut");
            it = garbageQueue.erase(it);
        } else
            ++it;
    }

    // move all task that will exceed the deadline to garbage queue
    it = wholeTaskWaitingQueue.begin();
    while (it != wholeTaskWaitingQueue.end() &&
            omnetpp::simTime() + (*it)->getRequiredCycle() / serverFrequency >
                (*it)->getCreationTime() + (*it)->getDelayTolerance()) {

        garbageQueue.push_back(*it);
        ++it;
        wholeTaskWaitingQueue.pop_front();
    }

    // move all task that will exceed the deadline to garbage queue
    it = subTaskWaitingQueue.begin();
    while (it != subTaskWaitingQueue.end() &&
            omnetpp::simTime() + (*it)->getRequiredCycle() / serverFrequency >
                (*it)->getCreationTime() + (*it)->getDelayTolerance()) {

        garbageQueue.push_back(*it);
        ++it;
        subTaskWaitingQueue.pop_front();
    }

    std::list<Task*>::iterator wholeTaskIt = wholeTaskWaitingQueue.begin();
    std::list<Task*>::iterator subTaskIt = subTaskWaitingQueue.begin();
    if (wholeTaskIt != wholeTaskWaitingQueue.end() && subTaskIt != subTaskWaitingQueue.end()) {
        // if subtask can meet the deadline after running the wholeTask first then run the whole task
        if (omnetpp::simTime() + ((*wholeTaskIt)->getRequiredCycle() + (*subTaskIt)->getRequiredCycle())
                / serverFrequency <= (*subTaskIt)->getCreationTime() + (*subTaskIt)->getDelayTolerance()) {
            runningTask = *wholeTaskIt;
            wholeTaskWaitingQueue.pop_front();

        } else {
            runningTask = *subTaskIt;
            subTaskWaitingQueue.pop_front();
        }
    } else if (wholeTaskIt != wholeTaskWaitingQueue.end() && subTaskIt == subTaskWaitingQueue.end()) {
        runningTask = *wholeTaskIt;
        wholeTaskWaitingQueue.pop_front();
    } else if (subTaskIt != subTaskWaitingQueue.end() && wholeTaskIt == wholeTaskWaitingQueue.end()) {
        runningTask = *subTaskIt;
        subTaskWaitingQueue.pop_front();
    }
}

ServerStatus *TaskQueue::getServerStatus() {
    Enter_Method("getServerStatus()");
    double requiredCycle = totalRequiredCycle;
    if (runningTask) {
        requiredCycle = totalRequiredCycle - (omnetpp::simTime() - runningTask->getSendingTime()).dbl() * serverFrequency;
    }
    ServerStatus *serverStatus = new ServerStatus(serverId, serverFrequency, serverMemory,
            requiredCycle, totalMemoryConsumed);

    return serverStatus;
}

void TaskQueue::refreshDisplay() const
{

}

void TaskQueue::finish()
{
}
