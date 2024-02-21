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

    for (std::list<Task*>::iterator it = waitingQueue.begin(); it != waitingQueue.end(); ++it)
        cancelAndDelete(*it);

    for (std::list<Task*>::iterator it = garbageQueue.begin(); it != garbageQueue.end(); ++it)
        cancelAndDelete(*it);

    if (runningTask)
        cancelAndDelete(runningTask);

    cancelAndDelete(serverInfoSignalMsg);
}

void TaskQueue::initialize()
{
    // server basic info
    serverId = getParentModule()->getIndex();
    int outputGateSize = getParentModule()->gateSize("ports$o");
    for (int i = 0; i < outputGateSize; ++i){
        int neighborServerIndex = getParentModule()->gate("ports$o", i)->getNextGate()->getOwnerModule()->getIndex();
        neighborServers.push_back(neighborServerIndex);
    }

    // initialize server capacity
//    double randFrequency = par("minFrequency").doubleValue() +
//            intrand(par("maxFrequency").doubleValue() - par("minFrequency").doubleValue() + 1);
//    serverFrequency = static_cast<double>(randFrequency) * 1e9;

    double randFrequency = par("minFrequency").doubleValue() +
            intrand(par("maxFrequency").doubleValue() - par("minFrequency").doubleValue() + 1);
    serverFrequency = randFrequency;

//    serverMemory = serverFrequency * par("memoryMultiple").doubleValue();

    double randMemory = par("minMemory").doubleValue() +
            intrand(par("maxMemory").doubleValue() - par("minMemory").doubleValue() + 1);
    serverMemory = randMemory;

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
    serverInfoSignalMsg = new omnetpp::cMessage("serverInfoSignalMsg");
    totalTaskFailed = 0;
    totalTaskCompleted = 0;
    memoryLoadingSignal = registerSignal("memoryLoading");
    CPULoadingSignal = registerSignal("CPULoading");
    totalTaskSignal = registerSignal("totalTask");
    totalTaskCompletedSignal = registerSignal("totalTaskCompleted");
    totalTaskFailedSignal = registerSignal("totalTaskFailed");
    scheduleAt(0, serverInfoSignalMsg);

}

void TaskQueue::handleMessage(omnetpp::cMessage *msg)
{
    if (msg == serverInfoSignalMsg) {
        EV << "serverMemory: " << serverMemory << omnetpp::endl;
        EV << "totalMemoryConsumed: " << totalMemoryConsumed << omnetpp::endl;

        EV << "serverFrequency: " << serverFrequency << omnetpp::endl;
        EV << "totalRequiredCycle: " << serverMemory << omnetpp::endl;

        emit(memoryLoadingSignal, totalMemoryConsumed / serverMemory);
        emit(CPULoadingSignal, totalRequiredCycle / serverFrequency);
        emit(totalTaskSignal, totalTaskFailed + totalTaskCompleted);
        emit(totalTaskCompletedSignal, totalTaskCompleted);
        emit(totalTaskFailedSignal, totalTaskFailed);
        totalTaskFailed = 0;
        totalTaskCompleted = 0;
        scheduleAfter(1, msg);
        return;
    }

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
        totalTaskCompleted += 1;
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
        totalTaskFailed += 1;
        send(incomingTask, "taskFinishedOut");
        return;
    }

    // if the current server can accommodate the task then send to the processor
    // otherwise, randomly offload to neighbor server
//    omnetpp::simtime_t predictedFinishedTime;
//    if (runningTask) {
//        predictedFinishedTime = runningTask->getArrivalTime() +
//                (totalRequiredCycle - runningTask->getRequiredCycle() + incomingTask->getRequiredCycle()) / serverFrequency;
//    } else {
//        predictedFinishedTime = omnetpp::simTime() + (totalRequiredCycle + incomingTask->getRequiredCycle()) / serverFrequency;
//    }
//    if (serverMemory - totalMemoryConsumed >= incomingTask->getTaskSize() &&
//            predictedFinishedTime <= incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
//        totalRequiredCycle += incomingTask->getRequiredCycle();
//        totalMemoryConsumed += incomingTask->getTaskSize();
//        FIFOwaitingQueue.push_back(incomingTask);
//    } else {
//        int randInt = intrand(neighborServers.size());
//        incomingTask->setDestinationServer(neighborServers[randInt]);
//        send(incomingTask, "offloadOut");
//    }

    if (serverMemory - totalMemoryConsumed >= incomingTask->getTaskSize()) {
        totalRequiredCycle += incomingTask->getRequiredCycle();
        totalMemoryConsumed += incomingTask->getTaskSize();
        FIFOwaitingQueue.push_back(incomingTask);
    } else {
        int randInt = intrand(neighborServers.size());
        incomingTask->setDestinationServer(neighborServers[randInt]);
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
        totalTaskFailed += 1;
        return;
    }

//    // if the current server can accommodate the task, then send to the processor
//    omnetpp::simtime_t predictedFinishedTime;
//    if (runningTask) {
//        predictedFinishedTime = runningTask->getArrivalTime() +
//                (totalRequiredCycle - runningTask->getRequiredCycle() + incomingTask->getRequiredCycle()) / serverFrequency;
//    } else {
//        predictedFinishedTime = omnetpp::simTime() + (totalRequiredCycle + incomingTask->getRequiredCycle()) / serverFrequency;
//    }
//
//    if (serverMemory - totalMemoryConsumed >= incomingTask->getTaskSize() &&
//            predictedFinishedTime <= incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
//        totalRequiredCycle += incomingTask->getRequiredCycle();
//        totalMemoryConsumed += incomingTask->getTaskSize();
//        FIFOwaitingQueue.push_back(incomingTask);
//    } else {
//
//        // get neighbor server status
//        std::vector<ServerStatus*> neighborServerStatus;
//        int outputGateSize = getParentModule()->gateSize("ports$o");
//        TaskQueue *neighborTaskQueueModule;
//        ServerStatus *curNeighborStatus;
//        for (int i = 0; i < outputGateSize; ++i){
//            neighborTaskQueueModule  = omnetpp::check_and_cast<TaskQueue*>(getParentModule()->gate("ports$o", i)->getNextGate()->getOwnerModule()->getSubmodule("taskQueue"));
//            curNeighborStatus = neighborTaskQueueModule->getServerStatus();
//            neighborServerStatus.push_back(curNeighborStatus);
//        }
//
//        std::sort(neighborServerStatus.begin(), neighborServerStatus.end(), [=](ServerStatus *lhs, ServerStatus *rhs) {
//            double lhsPredictedDelay = (lhs->getTotalRequiredCycle() + incomingTask->getRequiredCycle()) / lhs->getServerFrequency();
//            double rhsPredictedDelay = (rhs->getTotalRequiredCycle() + incomingTask->getRequiredCycle()) / rhs->getServerFrequency();
//
//            return lhsPredictedDelay < rhsPredictedDelay;
//        });
//
//        int bestNeighborServerId = 0;
//        for (int i = 0; i < neighborServerStatus.size(); ++i) {
//            if (neighborServerStatus[i]->getTotalMemoryConsumed() + incomingTask->getTaskSize() <= neighborServerStatus[i]->getServerMemory()) {
//                bestNeighborServerId = i;
//                break;
//            }
//        }
//
//        incomingTask->setDestinationServer(neighborServerStatus[bestNeighborServerId]->getServerId());
//        send(incomingTask, "offloadOut");
//
//        for (auto& status: neighborServerStatus)
//            delete status;
//    }
//

    if (serverMemory - totalMemoryConsumed >= incomingTask->getTaskSize()) {
        totalRequiredCycle += incomingTask->getRequiredCycle();
        totalMemoryConsumed += incomingTask->getTaskSize();
        FIFOwaitingQueue.push_back(incomingTask);
    } else {

        // get neighbor server status
        std::vector<ServerStatus*> neighborServerStatusVec;
        for (int i = 0; i < neighborServers.size(); ++i)
            neighborServerStatusVec.push_back(getServerStatus(neighborServers[i]));

        std::sort(neighborServerStatusVec.begin(), neighborServerStatusVec.end(), [](ServerStatus *lhs, ServerStatus *rhs) {
            return lhs->getServerMemory() - lhs->getTotalMemoryConsumed() > rhs->getServerMemory() - rhs->getTotalMemoryConsumed();
        });

        incomingTask->setDestinationServer(neighborServerStatusVec[0]->getServerId());
        send(incomingTask, "offloadOut");

        for (auto& status: neighborServerStatusVec)
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
        totalTaskFailed += 1;
        return;
    }


    omnetpp::simtime_t incomingTaskDeadline = incomingTask->getCreationTime() + incomingTask->getDelayTolerance();

    // incoming task is already splitted, just put it in this server
    if (incomingTask->getSubTaskVec().size() != incomingTask->getTotalSubTaskCount() &&
            totalMemoryConsumed + incomingTask->getTaskSize() <= serverMemory) {
        std::list<Task*>::reverse_iterator rit = waitingQueue.rbegin();
        while(rit != waitingQueue.rend() && (*rit)->getCreationTime() + (*rit)->getDelayTolerance() > incomingTaskDeadline)
            ++rit;

        waitingQueue.insert(rit.base(), incomingTask);
        totalRequiredCycle += incomingTask->getRequiredCycle();
        totalMemoryConsumed += incomingTask->getTaskSize();
        return;
    }


    ServerStatus *thisServerStatus = getServerStatus();
    std::vector<ServerStatus*> neighborServerStatusVec;
    for (int i = 0; i < neighborServers.size(); ++i)
        neighborServerStatusVec.push_back(getServerStatus(neighborServers[i]));

    std::sort(neighborServerStatusVec.begin(), neighborServerStatusVec.end(), [](ServerStatus *lhs, ServerStatus *rhs) {
        return lhs->getTotalRequiredCycle() / lhs->getServerFrequency() < rhs->getTotalRequiredCycle() / rhs->getServerFrequency();
    });

    double totalNeighborFrequency = 0.0;
    double totalNeighborRemainedSpace = 0.0;
    double totalNeighborRequiredCPUCycle = 0.0;
    double totalNeighborFinishedTime = 0.0;
    for (int i = 0; i < neighborServerStatusVec.size(); ++i) {
        totalNeighborFrequency += neighborServerStatusVec[i]->getServerFrequency();
        totalNeighborRemainedSpace += neighborServerStatusVec[i]->getServerMemory() - neighborServerStatusVec[i]->getTotalMemoryConsumed();
        totalNeighborRequiredCPUCycle += neighborServerStatusVec[i]->getTotalRequiredCycle();
        totalNeighborFinishedTime += neighborServerStatusVec[i]->getTotalRequiredCycle() / neighborServerStatusVec[i]->getServerFrequency();
    }


    bool isIncomingTaskSplittable = incomingTask->getSubTaskVec().size() > 1;

    double avgServerRequiredCPUCycle = (totalNeighborRequiredCPUCycle + thisServerStatus->getTotalRequiredCycle()) / (neighborServers.size() + 1);
    double avgServerFrequency = (totalNeighborFrequency + thisServerStatus->getServerFrequency()) / (neighborServers.size() + 1);
    double avgServerRemainedSpace = (totalNeighborRemainedSpace + (thisServerStatus->getServerMemory() - thisServerStatus->getTotalMemoryConsumed())) / (neighborServers.size() + 1);
    double avgServerFinishedTime = (totalNeighborFinishedTime + (thisServerStatus->getTotalRequiredCycle() / thisServerStatus->getServerFrequency())) / (neighborServers.size() + 1);

    double avgSubTaskRequiredCPUCycle = (incomingTask->getSubTaskVec().size() < neighborServers.size() + 1) ? incomingTask->getRequiredCycle() / incomingTask->getSubTaskVec().size()
            : incomingTask->getRequiredCycle() / (neighborServers.size() + 1);
    double avgSubTaskSize = (incomingTask->getSubTaskVec().size() < neighborServers.size() + 1) ? incomingTask->getTaskSize() / incomingTask->getSubTaskVec().size() :
            incomingTask->getTaskSize() / (neighborServers.size() + 1);

    double multipleForEnsuringTaskCanPlace = 1;
    double predictedTimeInTranmit = 5 * 1e-3;
    bool canPlaceAllSubTask = avgSubTaskSize * multipleForEnsuringTaskCanPlace <= avgServerRemainedSpace;
    bool canFinishAllSubTask = omnetpp::simTime() + predictedTimeInTranmit + (avgServerRequiredCPUCycle + avgSubTaskRequiredCPUCycle) / avgServerFrequency <= incomingTaskDeadline;

    if (isIncomingTaskSplittable && canPlaceAllSubTask && canFinishAllSubTask) {
        std::vector<ServerStatus*> serverStatusVec(neighborServerStatusVec);
        serverStatusVec.push_back(thisServerStatus);
        std::sort(serverStatusVec.begin(), serverStatusVec.end(), [](ServerStatus *lhs,ServerStatus *rhs) {
            return lhs->getTotalRequiredCycle() / lhs->getServerFrequency() < rhs->getTotalRequiredCycle() / rhs->getServerFrequency();
        });

        std::vector<Task*> taskToOffload;
        const subTaskVector& subTaskVec = incomingTask->getSubTaskVec();
        for (int i = 0; i < subTaskVec.size(); ++i)
            taskToOffload.push_back(createSubTask(incomingTask, i));
        cancelAndDelete(incomingTask);

        std::sort(taskToOffload.begin(), taskToOffload.end(), [](const Task *lhs, const Task *rhs) {
            return lhs->getRequiredCycle() > rhs->getRequiredCycle();
        });


        int curServerStatusIdx = 0;
        for (Task *task: taskToOffload) {
            if (serverStatusVec[curServerStatusIdx]->getServerId() == serverId && totalMemoryConsumed + task->getTaskSize() > serverMemory)
                curServerStatusIdx = (curServerStatusIdx + 1) % serverStatusVec.size();

            if (serverStatusVec[curServerStatusIdx]->getServerId() == serverId) {
                std::list<Task*>::reverse_iterator rit = waitingQueue.rbegin();
                while(rit != waitingQueue.rend() && (*rit)->getCreationTime() + (*rit)->getDelayTolerance() > incomingTaskDeadline)
                    ++rit;

                waitingQueue.insert(rit.base(), task);
                totalRequiredCycle += task->getRequiredCycle();
                totalMemoryConsumed += task->getTaskSize();
            } else {
                task->setDestinationServer(serverStatusVec[curServerStatusIdx]->getServerId());
                send(task, "offloadOut");
            }
            curServerStatusIdx = (curServerStatusIdx + 1) % serverStatusVec.size();
        }
        return;
    }



    bool canThisServerFinishTask = omnetpp::simTime() + (thisServerStatus->getTotalRequiredCycle() + incomingTask->getRequiredCycle()) / thisServerStatus->getServerFrequency()
            <= incomingTaskDeadline;
    bool canThisServerPlaceTask = thisServerStatus->getTotalMemoryConsumed() + incomingTask->getTaskSize() <= serverMemory;
    bool canBestNeighborFinishTask = omnetpp::simTime() + (neighborServerStatusVec[0]->getTotalRequiredCycle() + incomingTask->getRequiredCycle()) / neighborServerStatusVec[0]->getServerFrequency()
            <= incomingTaskDeadline;
    bool thisServerFinishedTimeLessThanNeighborServerFinishedTime = thisServerStatus->getTotalRequiredCycle() / thisServerStatus->getServerFrequency() <=
            totalNeighborRequiredCPUCycle / totalNeighborFrequency;

    if (canThisServerFinishTask && canThisServerPlaceTask && thisServerFinishedTimeLessThanNeighborServerFinishedTime) {
        std::list<Task*>::reverse_iterator rit = waitingQueue.rbegin();
        while(rit != waitingQueue.rend() && (*rit)->getCreationTime() + (*rit)->getDelayTolerance() > incomingTaskDeadline)
            ++rit;

        waitingQueue.insert(rit.base(), incomingTask);
        totalRequiredCycle += incomingTask->getRequiredCycle();
        totalMemoryConsumed += incomingTask->getTaskSize();
    } else if (canThisServerFinishTask && canThisServerPlaceTask &&
            !thisServerFinishedTimeLessThanNeighborServerFinishedTime && !canBestNeighborFinishTask) {
        std::list<Task*>::reverse_iterator rit = waitingQueue.rbegin();
        while(rit != waitingQueue.rend() && (*rit)->getCreationTime() + (*rit)->getDelayTolerance() > incomingTaskDeadline)
            ++rit;

        waitingQueue.insert(rit.base(), incomingTask);
        totalRequiredCycle += incomingTask->getRequiredCycle();
        totalMemoryConsumed += incomingTask->getTaskSize();
    } else {
        incomingTask->setDestinationServer(neighborServerStatusVec[0]->getServerId());
        send(incomingTask, "offloadOut");
    }


}

void TaskQueue::proposedDispatchingAlgo2(omnetpp::cMessage *msg) {
    /***********************************************
     *           Proposed Method                   *
     ***********************************************/
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        intVector& hopPathForUpdate = incomingTask->getHopPathForUpdate();
        hopPathForUpdate.push_back(serverId);
        send(incomingTask, "taskFinishedOut");
        totalTaskFailed += 1;
        return;
    }

    std::vector<Task*> taskToOffload;
//    taskToOffload.push_back(incomingTask);


    if (incomingTask->getSubTaskVec().size() == 1)
        taskToOffload.push_back(incomingTask);
    else {
        const subTaskVector& subTaskVec = incomingTask->getSubTaskVec();
        Task *curSubTask;
        for (int i = 0; i < subTaskVec.size(); ++i) {
            curSubTask = createSubTask(incomingTask, i);
            taskToOffload.push_back(curSubTask);
        }
        cancelAndDelete(incomingTask);
    }

    std::vector<ServerStatus*> neighborServerStatusVec;
    for (int i = 0; i < neighborServers.size(); ++i)
        neighborServerStatusVec.push_back(getServerStatus(neighborServers[i]));



    for (Task *task: taskToOffload) {
        double totalNeighborLoad = 0.0;
        double neighborLoad ;
        double bestNeighborLoad;
        int bestNeighborServerIdx = -1;
        for (int i = 0; i < neighborServerStatusVec.size(); ++i) {
            neighborLoad = neighborServerStatusVec[i]->getTotalRequiredCycle() / neighborServerStatusVec[i]->getServerFrequency();
            totalNeighborLoad += neighborLoad;
            if (bestNeighborServerIdx == -1 || neighborLoad < bestNeighborLoad) {
                bestNeighborServerIdx = i;
                bestNeighborLoad = neighborLoad;
            }
        }
        double avgNeighborLoad = totalNeighborLoad / neighborServers.size();

        double thisServerCurRequiredCycle = (runningTask != nullptr) ?
                totalRequiredCycle - (omnetpp::simTime() - runningTask->getSendingTime()).dbl() * serverFrequency
                : totalRequiredCycle;
        double thisServerLoad = thisServerCurRequiredCycle / serverFrequency;

        omnetpp::simtime_t taskDeadline = task->getCreationTime() + task->getDelayTolerance();

        bool canThisServerFinishTask = omnetpp::simTime() + (thisServerCurRequiredCycle + task->getRequiredCycle()) / serverFrequency <= taskDeadline;
        bool canThisServerPlaceTask = totalMemoryConsumed + task->getTaskSize() <= serverMemory;
        bool thisServerLoadLessThanNeighborLoad = thisServerLoad <= avgNeighborLoad;
        bool canBestNeighborServerFinishTask = omnetpp::simTime() + 5 * 1e-3 +
                (neighborServerStatusVec[bestNeighborServerIdx]->getTotalRequiredCycle() + task->getRequiredCycle()) / neighborServerStatusVec[bestNeighborServerIdx]->getServerFrequency()
                <= taskDeadline;

        if (canThisServerFinishTask && canThisServerPlaceTask && thisServerLoadLessThanNeighborLoad) {
            std::list<Task*>::reverse_iterator rit = waitingQueue.rbegin();
            while(rit != waitingQueue.rend() && (*rit)->getCreationTime() + (*rit)->getDelayTolerance() > taskDeadline)
                ++rit;

            waitingQueue.insert(rit.base(), task);
            totalRequiredCycle += task->getRequiredCycle();
            totalMemoryConsumed += task->getTaskSize();
        } else if (canThisServerFinishTask && canThisServerPlaceTask &&
                !thisServerLoadLessThanNeighborLoad && !canBestNeighborServerFinishTask) {
            std::list<Task*>::reverse_iterator rit = waitingQueue.rbegin();
            while(rit != waitingQueue.rend() && (*rit)->getCreationTime() + (*rit)->getDelayTolerance() > taskDeadline)
                ++rit;

            waitingQueue.insert(rit.base(), task);
            totalRequiredCycle += task->getRequiredCycle();
            totalMemoryConsumed += task->getTaskSize();
        } else {
            task->setDestinationServer(neighborServerStatusVec[bestNeighborServerIdx]->getServerId());
            neighborServerStatusVec[bestNeighborServerIdx]->setTotalRequiredCycle(neighborServerStatusVec[bestNeighborServerIdx]->getTotalRequiredCycle() + task->getRequiredCycle());
            neighborServerStatusVec[bestNeighborServerIdx]->setTotalMemoryConsumed(neighborServerStatusVec[bestNeighborServerIdx]->getTotalMemoryConsumed() + task->getTaskSize());
            send(task, "offloadOut");
        }
    }


//    auto compare = [=](ServerStatus* lhs, ServerStatus* rhs) {
//        double lhsPredictedDelay = (lhs->getTotalRequiredCycle() + task->getRequiredCycle()) / lhs->getServerFrequency();
//        double rhsPredictedDelay = (rhs->getTotalRequiredCycle() + task->getRequiredCycle()) / rhs->getServerFrequency();
//        return lhsPredictedDelay > rhsPredictedDelay;
//    };
//
//    std::priority_queue<ServerStatus*, std::vector<ServerStatus*>, decltype(compare)> serverStatusHeap(compare);
//    serverStatusHeap.push(thisServerStatus);
//    for (ServerStatus* status: neighborServerStatus)
//        serverStatusHeap.push(status);

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
            totalTaskFailed += 1;
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
            totalTaskFailed += 1;
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
            totalTaskFailed += 1;
            it = garbageQueue.erase(it);
        } else
            ++it;
    }

    // move all task that will exceed the deadline to garbage queue
    it = waitingQueue.begin();
    while (it != waitingQueue.end() &&
            omnetpp::simTime() + (*it)->getRequiredCycle() / serverFrequency >
                (*it)->getCreationTime() + (*it)->getDelayTolerance()) {

        garbageQueue.push_back(*it);
        ++it;
        waitingQueue.pop_front();
    }

    if (it != waitingQueue.end()) {
        runningTask = *it;
        waitingQueue.pop_front();
        double runningTime = runningTask->getRequiredCycle() / serverFrequency;
        intVector& hopPathForUpdate = runningTask->getHopPathForUpdate();
        hopPathForUpdate.push_back(serverId);
        runningTask->setRunningServer(serverId);
        scheduleAt(omnetpp::simTime() + runningTime, runningTask);
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

ServerStatus *TaskQueue::getServerStatus(int serverId) {
    TaskQueue *taskQueueModule = omnetpp::check_and_cast<TaskQueue*>(getSystemModule()->getSubmodule("edgeServer", serverId)->getSubmodule("taskQueue"));
    ServerStatus *serverStatus = taskQueueModule->getServerStatus();
    return serverStatus;
}

Task *TaskQueue::createSubTask(Task *task, int subTaskIdx) {
    Task *dupTask = task->dup();

    subTaskVector& subTaskVec = task->getSubTaskVecForUpdate();

    dupTask->setTaskSize(subTaskVec[subTaskIdx]->getSubTaskSize());

    dupTask->setRequiredCycle(subTaskVec[subTaskIdx]->getSubTaskRequiredCPUCycle());

    std::vector<int> hopPath(task->getHopPath());
    dupTask->setHopPath(hopPath);
    dupTask->setSubTaskVec({subTaskVec[subTaskIdx]});
    return dupTask;
}

double TaskQueue::getRegionLoad(int exceptServerId) {
    std::vector<int> servers;
    for (int i = 0; i < neighborServers.size(); ++i) {
        if (neighborServers[i] == exceptServerId)
            continue;
        servers.push_back(neighborServers[i]);
    }

    double regionLoad = TaskQueue::getAvgLoad(servers);
    return regionLoad;
}

double TaskQueue::getAvgLoad(const std::vector<int>& servers) {
    Enter_Method("getAvgLoad()");

    double avgLoad;
    TaskQueue *taskQueueModule;
    ServerStatus *serverStatus;
    for (int i = 0; i < servers.size(); ++i) {
        taskQueueModule = omnetpp::check_and_cast<TaskQueue*>(getSystemModule()->getSubmodule("edgeServer", servers[i])->getSubmodule("taskQueue"));
        serverStatus = taskQueueModule->getServerStatus();
        avgLoad = serverStatus->getTotalRequiredCycle() / serverStatus->getServerFrequency();
        delete serverStatus;
        delete taskQueueModule;
    }
    avgLoad /= servers.size();

    return avgLoad;
}

void TaskQueue::refreshDisplay() const
{

}

void TaskQueue::finish()
{
}
