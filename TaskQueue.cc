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

    if (dispatchingAlgo == "Local") {
        localDispatchingAlgo(msg);
    } else if (dispatchingAlgo == "Random") {
        randomDispatchingAlgo(msg);
    } else if (dispatchingAlgo == "Random2") {
        randomDispatchingAlgo2(msg);
    } else if (dispatchingAlgo == "Greedy") {
        greedyDispatchingAlgo(msg);
    } else if (dispatchingAlgo == "Greedy2") {
        greedyDispatchingAlgo2(msg);
    } else if (dispatchingAlgo == "Proposed") {
        proposedDispatchingAlgo(msg);
    } else if (dispatchingAlgo == "Proposed2") {
        proposedDispatchingAlgo2(msg);
    }

    scheduling();

}

void TaskQueue::localDispatchingAlgo(omnetpp::cMessage *msg) {
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    if (serverMemory - totalMemoryConsumed >= incomingTask->getTaskSize()) {
        insertTaskIntoWaitingQueue(incomingTask);
    } else {
        totalTaskFailed += 1;
        send(incomingTask, "taskFinishedOut");
    }
}

void TaskQueue::randomDispatchingAlgo(omnetpp::cMessage *msg) {

    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed the deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        totalTaskFailed += 1;
        send(incomingTask, "taskFinishedOut");
        return;
    }

    if (serverMemory - totalMemoryConsumed >= incomingTask->getTaskSize()) {
        insertTaskIntoWaitingQueue(incomingTask);
    } else {
        int randInt = intrand(neighborServers.size());
        incomingTask->setDestinationServer(neighborServers[randInt]);
        send(incomingTask, "offloadOut");
    }
}

void TaskQueue::randomDispatchingAlgo2(omnetpp::cMessage *msg) {


    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed the deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        totalTaskFailed += 1;
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
        insertTaskIntoWaitingQueue(incomingTask);
    } else {
        int randInt = intrand(neighborServers.size());
        incomingTask->setDestinationServer(neighborServers[randInt]);
        send(incomingTask, "offloadOut");
    }

}

void TaskQueue::greedyDispatchingAlgo(omnetpp::cMessage *msg) {

    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed the deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        send(incomingTask, "taskFinishedOut");
        totalTaskFailed += 1;
        return;
    }

    if (serverMemory - totalMemoryConsumed >= incomingTask->getTaskSize()) {
        insertTaskIntoWaitingQueue(incomingTask);
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

void TaskQueue::greedyDispatchingAlgo2(omnetpp::cMessage *msg) {

    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed the deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        send(incomingTask, "taskFinishedOut");
        totalTaskFailed += 1;
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
        insertTaskIntoWaitingQueue(incomingTask);
    } else {

        // get neighbor server status
        std::vector<ServerStatus*> neighborServerStatus;
        int outputGateSize = getParentModule()->gateSize("ports$o");
        TaskQueue *neighborTaskQueueModule;
        ServerStatus *curNeighborStatus;
        for (int i = 0; i < outputGateSize; ++i){
            neighborTaskQueueModule  = omnetpp::check_and_cast<TaskQueue*>(getParentModule()->gate("ports$o", i)->getNextGate()->getOwnerModule()->getSubmodule("taskQueue"));
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
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        send(incomingTask, "taskFinishedOut");
        totalTaskFailed += 1;
        return;
    }

    omnetpp::simtime_t incomingTaskDeadline = incomingTask->getCreationTime() + incomingTask->getDelayTolerance();
    if (incomingTask->getSubTaskVec().size() == 1) { // task cannot split

        std::vector<Task*> tasksToOffload = preemptive(incomingTask);
        for (Task *task: tasksToOffload)
            offloadTask(task);

    } else {  // task that can split
        subTaskVector& subTaskVec = incomingTask->getSubTaskVecForUpdate();

        std::vector<ServerStatus*> serverStatusVec;
        for (int i = 0; i < neighborServers.size(); ++i)
            serverStatusVec.push_back(getServerStatus(neighborServers[i]));
        serverStatusVec.push_back(getServerStatus());

        std::sort(serverStatusVec.begin(), serverStatusVec.end(), [](ServerStatus *lhs,ServerStatus *rhs) {
            return lhs->getTotalRequiredCycle() / lhs->getServerFrequency() < rhs->getTotalRequiredCycle() / rhs->getServerFrequency();
        });

        std::sort(subTaskVec.begin(), subTaskVec.end(), [](SubTask *lhs, SubTask *rhs) {
            return lhs->getSubTaskRequiredCPUCycle() > rhs->getSubTaskRequiredCPUCycle();
        });

        const double predictedTimeInTransmit = getSystemModule()->par("linkDelay").doubleValue();

        SubTask *curSubTask;
        int serverIdx = 0;
        int subTaskIdx = 0;
        while (subTaskIdx < subTaskVec.size() && serverIdx < serverStatusVec.size()) {
            curSubTask = subTaskVec[subTaskIdx];
            omnetpp::simtime_t predictedFinishedTime = (serverStatusVec[serverIdx]->getTotalRequiredCycle() + curSubTask->getSubTaskRequiredCPUCycle())
                    / serverStatusVec[serverIdx]->getServerFrequency() + omnetpp::simTime();
            predictedFinishedTime += (serverStatusVec[serverIdx]->getServerId() == serverId) ? 0 : predictedTimeInTransmit;
            if (predictedFinishedTime > incomingTaskDeadline)
                break;

            ++subTaskIdx;
            ++serverIdx;
        }
        bool isSplittingTask = subTaskIdx == subTaskVec.size();

        if (isSplittingTask) {
            std::vector<Task*> taskToOffload;
            for (int i = 0; i < subTaskVec.size(); ++i)
                taskToOffload.push_back(createSubTask(incomingTask, i));
            cancelAndDelete(incomingTask);

            serverIdx = 0;
            for (Task *task: taskToOffload) {
                if (serverStatusVec[serverIdx]->getServerId() == serverId) {
                    insertTaskIntoWaitingQueue(task);
                } else {
                    task->setDestinationServer(serverStatusVec[serverIdx]->getServerId());
                    send(task, "offloadOut");
                }
                ++serverIdx;
            }
        } else {
            offloadTask(incomingTask);
        } // end if we are going to split the task
    } // end if task can of can not split
}

void TaskQueue::proposedDispatchingAlgo2(omnetpp::cMessage *msg) {
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        send(incomingTask, "taskFinishedOut");
        totalTaskFailed += 1;
        return;
    }

    omnetpp::simtime_t incomingTaskDeadline = incomingTask->getCreationTime() + incomingTask->getDelayTolerance();
    if (incomingTask->getSubTaskVec().size() == 1) { // task cannot split

        std::vector<Task*> tasksToOffload = preemptive(incomingTask);
        for (Task *task: tasksToOffload)
            offloadTask(task);

    } else {  // task that can split
        subTaskVector& subTaskVec = incomingTask->getSubTaskVecForUpdate();

        std::vector<ServerStatus*> serverStatusVec;
        for (int i = 0; i < neighborServers.size(); ++i)
            serverStatusVec.push_back(getServerStatus(neighborServers[i]));
        serverStatusVec.push_back(getServerStatus());

        std::sort(serverStatusVec.begin(), serverStatusVec.end(), [](ServerStatus *lhs,ServerStatus *rhs) {
            return lhs->getTotalRequiredCycle() / lhs->getServerFrequency() < rhs->getTotalRequiredCycle() / rhs->getServerFrequency();
        });

        std::sort(subTaskVec.begin(), subTaskVec.end(), [](SubTask *lhs, SubTask *rhs) {
            return lhs->getSubTaskRequiredCPUCycle() > rhs->getSubTaskRequiredCPUCycle();
        });

        const double predictedTimeInTransmit = getSystemModule()->par("linkDelay").doubleValue();

        SubTask *curSubTask;
        int serverIdx = 0;
        int subTaskIdx = 0;
        while (subTaskIdx < subTaskVec.size() && serverIdx < serverStatusVec.size()) {
            curSubTask = subTaskVec[subTaskIdx];
            omnetpp::simtime_t predictedFinishedTime = (serverStatusVec[serverIdx]->getTotalRequiredCycle() + curSubTask->getSubTaskRequiredCPUCycle())
                    / serverStatusVec[serverIdx]->getServerFrequency() + omnetpp::simTime();
            predictedFinishedTime += (serverStatusVec[serverIdx]->getServerId() == serverId) ? 0 : predictedTimeInTransmit;
            if (predictedFinishedTime > incomingTaskDeadline)
                break;

            ++subTaskIdx;
            ++serverIdx;
        }
        bool isSplittingTask = subTaskIdx == subTaskVec.size();

        if (isSplittingTask) {
            std::vector<Task*> taskToOffload;
            for (int i = 0; i < subTaskVec.size(); ++i)
                taskToOffload.push_back(createSubTask(incomingTask, i));
            cancelAndDelete(incomingTask);

            serverIdx = 0;
            for (Task *task: taskToOffload) {
                if (serverStatusVec[serverIdx]->getServerId() == serverId) {
                    insertTaskIntoWaitingQueue(task);
                } else {
                    task->setDestinationServer(serverStatusVec[serverIdx]->getServerId());
                    send(task, "offloadOut");
                }
                ++serverIdx;
            }
        } else {
            offloadTask(incomingTask);
        } // end if we are going to split the task
    } // end if task can of can not split
}

std::vector<Task*> TaskQueue::preemptive(Task *task) {
    omnetpp::simtime_t taskDeadline = task->getCreationTime() + task->getDelayTolerance();

    if (dispatchingAlgo == "Proposed2") {
        if (task->getSubTaskVec().size() == task->getTotalSubTaskCount()) {
            ServerStatus *thisServerStatus = getServerStatus();
            std::vector<ServerStatus*> neighborServerStatusVec;
            for (int i = 0; i < neighborServers.size(); ++i)
                neighborServerStatusVec.push_back(getServerStatus(neighborServers[i]));

            double totalRequiredCycle = 0.0;
            double totalFrequency = 0.0;
            for (auto curServerStatus: neighborServerStatusVec) {
                totalRequiredCycle += curServerStatus->getTotalRequiredCycle();
                totalFrequency += curServerStatus->getServerFrequency();
            }
            const double predictedTimeInTransmit = getSystemModule()->par("linkDelay").doubleValue();

            omnetpp::simtime_t neighborAvgFinishedTime = omnetpp::simTime() + predictedTimeInTransmit + totalRequiredCycle / totalFrequency;
            double neighborAvgFrequency = totalFrequency / neighborServers.size();


            if (neighborAvgFinishedTime <
                    omnetpp::simTime() + thisServerStatus->getTotalRequiredCycle() / thisServerStatus->getServerFrequency())
                return {task};
        }
    }


    // calculate max spare time of waiting queue and incoming task
    double maxSpareTime = taskDeadline.dbl() - task->getRequiredCycle() / serverFrequency - omnetpp::simTime().dbl();
    double curSpareTime;
    for (std::list<Task*>::iterator it = waitingQueue.begin(); it != waitingQueue.end(); ++it) {
        curSpareTime = ((*it)->getCreationTime() + (*it)->getDelayTolerance() - (*it)->getRequiredCycle() / serverFrequency - omnetpp::simTime()).dbl();
        if (curSpareTime > maxSpareTime)
            maxSpareTime = curSpareTime;
    }

    // priority queue for all score higher than incoming task, and it's place in waiting queue is before incoming task
    auto compare = [=](std::list<Task*>::iterator lhs, std::list<Task*>::iterator rhs) {
        double lhsSpareTime = ((*lhs)->getCreationTime() + (*lhs)->getDelayTolerance() - (*lhs)->getRequiredCycle() / serverFrequency - omnetpp::simTime()).dbl();
        double lhsWholeTask = static_cast<double>((*lhs)->getSubTaskVec().size() == (*lhs)->getTotalSubTaskCount());
        double lhsScore = getScore(lhsSpareTime, maxSpareTime, lhsWholeTask);
        double rhsSpareTime = ((*rhs)->getCreationTime() + (*rhs)->getDelayTolerance() - (*rhs)->getRequiredCycle() / serverFrequency - omnetpp::simTime()).dbl();
        double rhsWholeTask = static_cast<double>((*rhs)->getSubTaskVec().size() == (*rhs)->getTotalSubTaskCount());
        double rhsScore = getScore(rhsSpareTime, maxSpareTime, rhsWholeTask);
        return lhsScore < rhsScore;
    };
    std::priority_queue<std::list<Task*>::iterator, std::vector<std::list<Task*>::iterator>, decltype(compare)> maxHeap(compare);


    // build the priority queue, and find the place that incoming task should be inserted
    double accuRequiredCycle = (runningTask != nullptr) ? runningTask->getRequiredCycle() - getCurrentRunningTaskFinishedCycle() : 0.0;
    double canReleaseCycle = 0.0;
    double taskSpareTime = taskDeadline.dbl() - task->getRequiredCycle() / serverFrequency - omnetpp::simTime().dbl();
    double taskWholeTask = static_cast<double>(task->getSubTaskVec().size() == task->getTotalSubTaskCount());
    double taskScore = getScore(taskSpareTime, maxSpareTime, taskWholeTask);
    double curWholeTask, curScore;
    omnetpp::simtime_t curDeadline;
    std::list<Task*>::iterator it = waitingQueue.begin();
    while (it != waitingQueue.end()) {
        curSpareTime = ((*it)->getCreationTime() + (*it)->getDelayTolerance() - (*it)->getRequiredCycle() / serverFrequency - omnetpp::simTime()).dbl();
        curWholeTask = static_cast<double>((*it)->getSubTaskVec().size() == (*it)->getTotalSubTaskCount());
        curScore = getScore(curSpareTime, maxSpareTime, curWholeTask);

        if (schedulingAlgo == "EDF") {
            if (curDeadline > taskDeadline)
                break;
        } else {
            if (curSpareTime > taskSpareTime)
                break;
        }

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
    if (omnetpp::simTime() + (accuRequiredCycle - canReleaseCycle + task->getRequiredCycle()) / serverFrequency > taskDeadline)
        taskToOffload.push_back(task);
    else {
        std::list<Task*>::iterator tmpIt;
        while (!maxHeap.empty() &&
                omnetpp::simTime() + (accuRequiredCycle + task->getRequiredCycle()) / serverFrequency > taskDeadline) {
            tmpIt = maxHeap.top();
            maxHeap.pop();
            accuRequiredCycle -= (*tmpIt)->getRequiredCycle();
            taskItrToOffload.push_back(tmpIt);
        }

        waitingQueue.insert(it, task);
        accuRequiredCycle += task->getRequiredCycle();
        totalRequiredCycle += task->getRequiredCycle();
        totalMemoryConsumed += task->getTaskSize();

        // when the incoming task be inserted into this server, it will impact all the task after it.
        // move all task wouldn't finished in time to taskItrToOffload
        while (it != waitingQueue.end()) {
            if (omnetpp::simTime() + (accuRequiredCycle + (*it)->getRequiredCycle()) / serverFrequency > (*it)->getCreationTime() + (*it)->getDelayTolerance())
                taskItrToOffload.push_back(it);
            else
                accuRequiredCycle += (*it)->getRequiredCycle();
            ++it;
        }

        Task *curTask;
        for (int i = 0; i < taskItrToOffload.size(); ++i) {
            curTask = *taskItrToOffload[i];
            waitingQueue.erase(taskItrToOffload[i]);
            taskToOffload.push_back(curTask);
            totalRequiredCycle -= curTask->getRequiredCycle();
            totalMemoryConsumed -= curTask->getTaskSize();
        }
    } // end if incoming task can be inserted into waiting Queue

    return taskToOffload;
}

void TaskQueue::offloadTask(Task *task) {
    std::vector<ServerStatus*> neighborServerStatusVec;

    for (int i = 0; i < neighborServers.size(); ++i)
        neighborServerStatusVec.push_back(getServerStatus(neighborServers[i]));

    std::sort(neighborServerStatusVec.begin(), neighborServerStatusVec.end(), [](ServerStatus *lhs, ServerStatus *rhs) {
        return lhs->getTotalRequiredCycle() / lhs->getServerFrequency() < rhs->getTotalRequiredCycle() / rhs->getServerFrequency();
    });

    int i = 0;
    while (task->getHopPath().size() >= 2 &&
            neighborServerStatusVec[i]->getServerId() == task->getHopPath()[task->getHopPath().size()-2])
        ++i;

    task->setDestinationServer(neighborServerStatusVec[i]->getServerId());
    send(task, "offloadOut");

}

double TaskQueue::getScore(double spareTime, double maxSpareTime, double wholeTask) {
    return 1 * spareTime / maxSpareTime + wholeTask;
}

void TaskQueue::scheduling() {
    if (runningTask)
        return;

    // because it's FIFO so, its not sorted by deadline, we need to remove the task in the back
    if (schedulingAlgo == "FIFO") {
        std::list<Task*>::iterator it = waitingQueue.begin();
        while (it != waitingQueue.end()) {
            if (omnetpp::simTime() > (*it)->getCreationTime() + (*it)->getDelayTolerance()) {
                totalRequiredCycle -= (*it)->getRequiredCycle();
                totalMemoryConsumed -= (*it)->getTaskSize();
                send((*it), "taskFinishedOut");
                totalTaskFailed += 1;
                it = waitingQueue.erase(it);
            } else
                ++it;
        }
    }


    // remove tasks that exceed deadline to task collector
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


    // garbage queue is used to collect the task not yet exceed the deadline, but can't be processed in time by the server
    // move tasks that will exceed the deadline to garbage queue
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
        runningTask->setRunningServer(serverId);
        scheduleAt(omnetpp::simTime() + runningTime, runningTask);
    }
}

void TaskQueue::insertTaskIntoWaitingQueue(Task *task) {

    if (schedulingAlgo == "FIFO") {
        waitingQueue.push_back(task);
        totalRequiredCycle += task->getRequiredCycle();
        totalMemoryConsumed += task->getTaskSize();
    } else if (schedulingAlgo == "EDF") {
        std::list<Task*>::reverse_iterator rit = waitingQueue.rbegin();
        omnetpp::simtime_t taskDeadline = task->getCreationTime() + task->getDelayTolerance();
        while(rit != waitingQueue.rend() && (*rit)->getCreationTime() + (*rit)->getDelayTolerance() > taskDeadline)
            ++rit;

        waitingQueue.insert(rit.base(), task);
        totalRequiredCycle += task->getRequiredCycle();
        totalMemoryConsumed += task->getTaskSize();
    } else if (schedulingAlgo == "SST") {   // SST (Shortest Spare Time) : deadline - processing - currentTime
        std::list<Task*>::reverse_iterator rit = waitingQueue.rbegin();
        omnetpp::simtime_t taskSpareTime = task->getCreationTime() + task->getDelayTolerance()
                - omnetpp::simTime() - task->getTaskSize() / serverFrequency;
        while(rit != waitingQueue.rend() && (*rit)->getCreationTime() + (*rit)->getDelayTolerance()
                - omnetpp::simTime() - (*rit)->getRequiredCycle() / serverFrequency > taskSpareTime)
            ++rit;
        waitingQueue.insert(rit.base(), task);
        totalRequiredCycle += task->getRequiredCycle();
        totalMemoryConsumed += task->getTaskSize();
    }
}

double TaskQueue::getCurrentRunningTaskFinishedCycle() {
    if (!runningTask)
        return 0;
    return (omnetpp::simTime() - runningTask->getSendingTime()).dbl() * serverFrequency;
}

ServerStatus *TaskQueue::getServerStatus() {
    Enter_Method("getServerStatus()");
    ServerStatus *serverStatus = new ServerStatus(serverId, serverFrequency, serverMemory,
            totalRequiredCycle - getCurrentRunningTaskFinishedCycle(), totalMemoryConsumed);

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

void TaskQueue::refreshDisplay() const {
}

void TaskQueue::finish() {
    for (std::list<Task*>::iterator it = waitingQueue.begin(); it != waitingQueue.end(); ++it)
        cancelAndDelete(*it);

    for (std::list<Task*>::iterator it = garbageQueue.begin(); it != garbageQueue.end(); ++it)
        cancelAndDelete(*it);

    if (runningTask)
        cancelAndDelete(runningTask);

    cancelAndDelete(serverInfoSignalMsg);
}
