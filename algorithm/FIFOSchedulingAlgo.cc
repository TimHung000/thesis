/*
 * FIFOSchedulingAlgo.cc
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */



#include "FIFOSchedulingAlgo.h"
#include "TaskQueue.h"

FIFOSchedulingAlgo::FIFOSchedulingAlgo(TaskQueue *taskQueue) {
    this->taskQueue = taskQueue;
}

FIFOSchedulingAlgo::~FIFOSchedulingAlgo() {}

void FIFOSchedulingAlgo::scheduling() {
    if (taskQueue->runningTask)
        return;

    std::list<Task*>::iterator it = taskQueue->waitingQueue.begin();
    while (it != taskQueue->waitingQueue.end()) {
        if (omnetpp::simTime() > (*it)->getCreationTime() + (*it)->getDelayTolerance()) {
            taskQueue->totalRequiredCycle -= (*it)->getRequiredCycle();
            taskQueue->totalMemoryConsumed -= (*it)->getTaskSize();
            taskQueue->send((*it), "taskFinishedOut");
            taskQueue->totalTaskFailed += 1;
            it = taskQueue->waitingQueue.erase(it);
        } else
            ++it;
    }

    // remove tasks that exceed deadline to task collector
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


    // garbage queue is used to collect the task not yet exceed the deadline, but can't be processed in time by the server
    // move tasks that will exceed the deadline to garbage queue
    it = taskQueue->waitingQueue.begin();
    while (it != taskQueue->waitingQueue.end() &&
            omnetpp::simTime() + (*it)->getRequiredCycle() / taskQueue->serverFrequency >
                (*it)->getCreationTime() + (*it)->getDelayTolerance()) {

        taskQueue->garbageQueue.push_back(*it);
        ++it;
        taskQueue->waitingQueue.pop_front();
    }

    if (it != taskQueue->waitingQueue.end()) {
        taskQueue->runningTask = *it;
        taskQueue->waitingQueue.pop_front();
        double runningTime = taskQueue->runningTask->getRequiredCycle() / taskQueue->serverFrequency;
        taskQueue->runningTask->setRunningServer(taskQueue->serverId);
        taskQueue->scheduleAt(omnetpp::simTime() + runningTime, taskQueue->runningTask);
    }
}

void FIFOSchedulingAlgo::insertTaskIntoWaitingQueue(Task *task) {
    std::list<Task*>::reverse_iterator rit = taskQueue->waitingQueue.rbegin();
    omnetpp::simtime_t taskDeadline = task->getCreationTime() + task->getDelayTolerance();
    while(rit != taskQueue->waitingQueue.rend() && (*rit)->getCreationTime() + (*rit)->getDelayTolerance() > taskDeadline)
        ++rit;

    taskQueue->waitingQueue.insert(rit.base(), task);
    taskQueue->totalRequiredCycle += task->getRequiredCycle();
    taskQueue->totalMemoryConsumed += task->getTaskSize();
}
