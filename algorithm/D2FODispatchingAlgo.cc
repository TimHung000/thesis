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


D2FODispatchingAlgo::D2FODispatchingAlgo(SchedulingAlgo *schedulingAlgo, TaskQueue *taskQueue) {
    this->schedulingAlgo = schedulingAlgo;
    this->taskQueue = taskQueue;
}

D2FODispatchingAlgo::~D2FODispatchingAlgo() {}

// use cpu as criteria
void D2FODispatchingAlgo::execute(omnetpp::cMessage *msg) {
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);
    omnetpp::simtime_t incomingTaskDeadline = incomingTask->getCreationTime() + incomingTask->getDelayTolerance();

    // task exceed the deadline
    if (omnetpp::simTime() > incomingTaskDeadline) {
        taskQueue->totalTaskFailed += 1;
        taskQueue->send(incomingTask, "taskFinishedOut");
        return;
    }

    // if the current server can accommodate the task, then send to the processor
    omnetpp::simtime_t predictedFinishedTime = omnetpp::simTime() +
            (taskQueue->totalRequiredCycle - taskQueue->getCurrentRunningTaskFinishedCycle() + incomingTask->getRequiredCycle())
                / taskQueue->serverFrequency;

    if (taskQueue->serverMemory - taskQueue->totalMemoryConsumed >= incomingTask->getTaskSize() &&
            predictedFinishedTime <= incomingTaskDeadline) {
        schedulingAlgo->insertTaskIntoWaitingQueue(incomingTask);
    } else {
        // get neighbor server status
        std::vector<ServerStatus*> neighborServerStatus;
        int outputGateSize = taskQueue->getParentModule()->gateSize("ports$o");
        TaskQueue *neighborTaskQueueModule;
        ServerStatus *curNeighborStatus;
        for (int i = 0; i < outputGateSize; ++i){
            neighborTaskQueueModule  = omnetpp::check_and_cast<TaskQueue*>(taskQueue->getParentModule()->gate("ports$o", i)->getNextGate()->getOwnerModule()->getSubmodule("taskQueue"));
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
        taskQueue->send(incomingTask, "offloadOut");

        for (auto& status: neighborServerStatus)
            delete status;
    }

    schedulingAlgo->scheduling();
}

//// use memory as criteria
//void GreedyDispatchingAlgo::execute(omnetpp::cMessage *msg) {
//    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);
//
//    if (serverMemory - totalMemoryConsumed >= incomingTask->getTaskSize()) {
//        insertTaskIntoWaitingQueue(incomingTask);
//    } else {
//
//        // get neighbor server status
//        std::vector<ServerStatus*> neighborServerStatusVec;
//        for (int i = 0; i < neighborServers.size(); ++i)
//            neighborServerStatusVec.push_back(getServerStatus(neighborServers[i]));
//
//        std::sort(neighborServerStatusVec.begin(), neighborServerStatusVec.end(), [](ServerStatus *lhs, ServerStatus *rhs) {
//            return lhs->getServerMemory() - lhs->getTotalMemoryConsumed() > rhs->getServerMemory() - rhs->getTotalMemoryConsumed();
//        });
//
//        incomingTask->setDestinationServer(neighborServerStatusVec[0]->getServerId());
//        send(incomingTask, "offloadOut");
//
//        for (auto& status: neighborServerStatusVec)
//            delete status;
//    }
//}
