/*
 * SubTask.cc
 *
 *  Created on: Feb 7, 2024
 *      Author: tim
 */

#include "SubTask.h"

SubTask::SubTask(int subTaskId, double subTaskSize, double subTaskRequiredCPUCycle):
    subTaskId(subTaskId), subTaskSize(subTaskSize), subTaskRequiredCPUCycle(subTaskRequiredCPUCycle){
}

SubTask::SubTask(SubTask *subTask):
    subTaskId(subTask->subTaskId), subTaskSize(subTask->subTaskSize), subTaskRequiredCPUCycle(subTask->subTaskRequiredCPUCycle){
}
int SubTask::getSubTaskId() const{
    return this->subTaskId;
}

double SubTask::getSubTaskSize() const{
    return this->subTaskSize;
}

double SubTask::getSubTaskRequiredCPUCycle() const{
    return this->subTaskRequiredCPUCycle;
}
