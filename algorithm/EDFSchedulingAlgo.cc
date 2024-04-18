/*
 * EDFSchedulingAlgo.cc
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#include "EDFSchedulingAlgo.h"
#include "TaskQueue.h"

EDFSchedulingAlgo::EDFSchedulingAlgo(TaskQueue *taskQueue) {
    this->taskQueue = taskQueue;
}

EDFSchedulingAlgo::~EDFSchedulingAlgo() {}

void EDFSchedulingAlgo::scheduling() {
    if (taskQueue->runningTask)
        return;

    cleanExpiredTask();


    // garbage queue is used to collect the task not yet exceed the deadline, but can't be processed in time by the server
    // move tasks that will exceed the deadline to garbage queue
    auto it = taskQueue->waitingQueue.begin();
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

std::list<Task*>::iterator EDFSchedulingAlgo::insertTaskIntoWaitingQueue(Task *task) {
    std::list<Task*>::iterator it = getInsertionPoint(task);
    it = insertTaskIntoWaitingQueue(task, it);
    return it;
}

std::list<Task*>::iterator EDFSchedulingAlgo::insertTaskIntoWaitingQueue(Task *task, std::list<Task*>::iterator it) {
    it = taskQueue->waitingQueue.insert(it, task);
    taskQueue->totalRequiredCycle += task->getRequiredCycle();
    taskQueue->totalMemoryConsumed += task->getTaskSize();
    return it;
}

std::list<Task*>::iterator EDFSchedulingAlgo::getInsertionPoint(Task *task) {
    std::list<Task*>::reverse_iterator rit = taskQueue->waitingQueue.rbegin();
    omnetpp::simtime_t taskDeadline = task->getCreationTime() + task->getDelayTolerance();
    while(rit != taskQueue->waitingQueue.rend() && (*rit)->getCreationTime() + (*rit)->getDelayTolerance() > taskDeadline)
        ++rit;
    return rit.base();
}

