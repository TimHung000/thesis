/*
 * utility.cc
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#include "Utility.h"

double getScore(double spareTime, double maxSpareTime, double wholeTask, double wholeRequiredCycle, double maxWholeRequiredCycle) {
    return 0.1 * spareTime / maxSpareTime + 0.4 * wholeRequiredCycle / maxWholeRequiredCycle + 0.5 * wholeTask;
}

Task *createSubTask(Task *task, int subTaskIdx) {
    Task *dupTask = task->dup();

    subTaskVector& subTaskVec = task->getSubTaskVecForUpdate();

    dupTask->setTaskSize(subTaskVec[subTaskIdx]->getSubTaskSize());

    dupTask->setRequiredCycle(subTaskVec[subTaskIdx]->getSubTaskRequiredCPUCycle());

    std::vector<int> hopPath(task->getHopPath());
    dupTask->setHopPath(hopPath);
    dupTask->setSubTaskVec({subTaskVec[subTaskIdx]});
    return dupTask;
}
