/*
 * Dispatcher.cc
 *
 *  Created on: Jan 29, 2024
 *      Author: tim
 */


#include <vector>
#include <random>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <limits>

#include "TaskQueue.h"

#include "Task_m.h"
#include "SubTask.h"
#include "ServerStatus.h"

#include "algorithm/LocalDispatchingAlgo.h"
#include "algorithm/RandomDispatchingAlgo.h"
#include "algorithm/D2FODispatchingAlgo.h"
#include "algorithm/RandomWalkLoadBalancingDispatchingAlgo.h"
#include "algorithm/ProposedDispatchingAlgo.h"
#include "algorithm/GreedyPartitionDispatchingAlgo.h"
#include "algorithm/RandomPartitionDispatchingAlgo.h"

#include "algorithm/FIFOSchedulingAlgo.h"
#include "algorithm/EDFSchedulingAlgo.h"
#include "algorithm/SSTSchedulingAlgo.h"


Define_Module(TaskQueue);

DispatchingAlgo *TaskQueue::getDispatchingAlgo(std::string name, SchedulingAlgo *schedulingAlgo, TaskQueue *taskQueue) {
    DispatchingAlgo *dispatchingAlgo = nullptr;
    if (name == "Local") {
        dispatchingAlgo = new LocalDispatchingAlgo(schedulingAlgo, taskQueue);
    }
    else if (name == "Random") {
        dispatchingAlgo = new RandomDispatchingAlgo(schedulingAlgo, taskQueue);
    }
    else if (name == "D2FO") {
        dispatchingAlgo = new D2FODispatchingAlgo(schedulingAlgo, taskQueue);
    }
    else if (name == "DLAGO-TP") {
        dispatchingAlgo = new ProposedDispatchingAlgo(schedulingAlgo, taskQueue);
    }
    else if (name == "RandomWalk") {
        dispatchingAlgo = new RandomWalkLoadBalancingDispatchingAlgo(schedulingAlgo, taskQueue);
    }
    else if (name == "GreedyPartition") {
        dispatchingAlgo = new GreedyPartitionDispatchingAlgo(schedulingAlgo, taskQueue);
    }
    else if (name == "RandomPartition") {
        dispatchingAlgo = new RandomPartitionDispatchingAlgo(schedulingAlgo, taskQueue);
    }
    else {
        dispatchingAlgo = new ProposedDispatchingAlgo(schedulingAlgo, taskQueue);
    }

    return dispatchingAlgo;
}

SchedulingAlgo *TaskQueue::getSchedulingAlgo(std::string name, TaskQueue *taskQueue) {
    SchedulingAlgo *schedulingAlgo = nullptr;
    if (name == "FIFO") {
        schedulingAlgo = new FIFOSchedulingAlgo(taskQueue);
    }
    else if (name == "EDF") {
        schedulingAlgo = new EDFSchedulingAlgo(taskQueue);
    }
    else if (name == "SST") {
        schedulingAlgo = new SSTSchedulingAlgo(taskQueue);
    } else {
        schedulingAlgo = new FIFOSchedulingAlgo(taskQueue);
    }
    return schedulingAlgo;
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

    // set server CPU frequency
    double randFrequency = par("minFrequency").doubleValue() +
            intrand(par("maxFrequency").doubleValue() - par("minFrequency").doubleValue() + 1);
    serverFrequency = randFrequency;

    // set server storage
    double randMemory = par("minMemory").doubleValue() +
            intrand(par("maxMemory").doubleValue() - par("minMemory").doubleValue() + 1);
    serverMemory = randMemory;

    // initialize server status
    totalRequiredCycle = 0;
    totalMemoryConsumed = 0;

    // for processing task management
    runningTask = nullptr;

    // get algorithm instance
    schedulingAlgo = getSchedulingAlgo(par("schedulingAlgo").stdstringValue(), this);
    dispatchingAlgo = getDispatchingAlgo(par("dispatchingAlgo").stdstringValue(), schedulingAlgo, this);

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
        schedulingAlgo->scheduling();
        return;
    }

    dispatchingAlgo->execute(msg);
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

double TaskQueue::getCurrentRunningTaskFinishedCycle() {
    if (!runningTask)
        return 0;
    return (omnetpp::simTime() - runningTask->getSendingTime()).dbl() * serverFrequency;
}

void TaskQueue::refreshDisplay() const {}

void TaskQueue::finish() {
    for (std::list<Task*>::iterator it = waitingQueue.begin(); it != waitingQueue.end(); ++it)
        cancelAndDelete(*it);

    for (std::list<Task*>::iterator it = garbageQueue.begin(); it != garbageQueue.end(); ++it)
        cancelAndDelete(*it);

    if (runningTask)
        cancelAndDelete(runningTask);

    cancelAndDelete(serverInfoSignalMsg);
    delete dispatchingAlgo;
    delete schedulingAlgo;
}

TaskQueue::~TaskQueue() {}
