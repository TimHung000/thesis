/*
 * PrioritySchedulingAlgo.cc
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#include "PrioritySchedulingAlgo.h"
#include "TaskQueue.h"
#include "Utility.h"


PrioritySchedulingAlgo::PrioritySchedulingAlgo(TaskQueue *taskQueue) {
    this->taskQueue = taskQueue;
}

PrioritySchedulingAlgo::~PrioritySchedulingAlgo() {}

void PrioritySchedulingAlgo::scheduling() {
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

std::list<Task*>::iterator PrioritySchedulingAlgo::insertTaskIntoWaitingQueue(Task *task) {
    std::list<Task*>::iterator it = getInsertionPoint(task);
    it = insertTaskIntoWaitingQueue(task, it);
    return it;
}

std::list<Task*>::iterator PrioritySchedulingAlgo::insertTaskIntoWaitingQueue(Task *task, std::list<Task*>::iterator it) {
    it = taskQueue->waitingQueue.insert(it, task);
    taskQueue->totalRequiredCycle += task->getRequiredCycle();
    taskQueue->totalMemoryConsumed += task->getTaskSize();
    return it;
}

std::list<Task*>::iterator PrioritySchedulingAlgo::getInsertionPoint(Task *task) {

    // calculate max spare time of waiting queue and incoming task
    double maxSpareTime = (task->getCreationTime() + task->getDelayTolerance()- task->getRequiredCycle() / taskQueue->serverFrequency - omnetpp::simTime()).dbl();
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


    std::list<Task*>::reverse_iterator rit = taskQueue->waitingQueue.rbegin();
    double taskPriority = getPriority((task->getCreationTime() + task->getDelayTolerance() - task->getRequiredCycle() / taskQueue->serverFrequency - omnetpp::simTime()).dbl(),
            maxSpareTime,
            task->getWholeRequiredCycle(),
            maxWholeRequiredCycle,
            (task->getSubTaskVec().size() == task->getTotalSubTaskCount()) ? 1.0 : 1.0 / static_cast<double>(task->getTotalSubTaskCount())
    );

    while(rit != taskQueue->waitingQueue.rend()) {
        double curPriority = getPriority(((*rit)->getCreationTime() + (*rit)->getDelayTolerance() - (*rit)->getRequiredCycle() / taskQueue->serverFrequency - omnetpp::simTime()).dbl(),
                maxSpareTime,
                (*rit)->getWholeRequiredCycle(),
                maxWholeRequiredCycle,
                ((*rit)->getSubTaskVec().size() == (*rit)->getTotalSubTaskCount()) ? 1.0 : 1.0 / static_cast<double>((*rit)->getTotalSubTaskCount())
        );

        if (curPriority <= taskPriority)
            break;
        ++rit;
    }
    return rit.base();
}

double PrioritySchedulingAlgo::getPriority(double spareTime, double maxSpareTime, double wholeRequiredCycle, double maxWholeRequiredCycle, double taskSplitFactor) {
    return 1 * spareTime / maxSpareTime + 0 * taskSplitFactor;
}

