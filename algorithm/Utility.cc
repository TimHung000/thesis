/*
 * utility.cc
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#include "Utility.h"

double getScore(double spareTime, double maxSpareTime, double wholeRequiredCycle, double maxWholeRequiredCycle, double taskSplitFactor) {
    return 0.2 * spareTime / maxSpareTime + 0.3 * wholeRequiredCycle / maxWholeRequiredCycle + 0.5 * taskSplitFactor;
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
