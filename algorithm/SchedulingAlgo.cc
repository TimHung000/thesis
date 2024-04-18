/*
 * SchedulingAlgo.cc
 *
 *  Created on: Apr 14, 2024
 *      Author: tim
 */

#include "SchedulingAlgo.h"
#include "TaskQueue.h"

void SchedulingAlgo::cleanExpiredTask() {

    auto it = taskQueue->waitingQueue.begin();
    while (it != taskQueue->waitingQueue.end()) {
        if (omnetpp::simTime() > (*it)->getCreationTime() + (*it)->getDelayTolerance()) {
            taskQueue->garbageQueue.push_back(*it);
            it = taskQueue->waitingQueue.erase(it);
        } else
            ++it;
    }

    it = taskQueue->garbageQueue.begin();
    while (it != taskQueue->garbageQueue.end()) {
        if (omnetpp::simTime() > (*it)->getCreationTime() + (*it)->getDelayTolerance()) {
            taskQueue->totalRequiredCycle -= (*it)->getRequiredCycle();
            taskQueue->totalMemoryConsumed -= (*it)->getTaskSize();
            taskQueue->send((*it), "taskFinishedOut");
            taskQueue->totalTaskFailed += 1;
            it = taskQueue->garbageQueue.erase(it);
        } else
            ++it;
    }
}
