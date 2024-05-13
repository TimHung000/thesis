    /*
 * TaskCollection.cc
 *
 *  Created on: Feb 4, 2024
 *      Author: tim
 */

#include "FinishedTaskCollector.h"

#include "Task_m.h"
#include <string>
#include <sstream>
#include <utility>

Define_Module(FinishedTaskCollector);

void FinishedTaskCollector::initialize()
{
    isCompletedSignal = registerSignal("isCompleted");
    splitTaskPartialCompleteSignal = registerSignal("splitTaskPartialComplete");
    splitTaskCompleteSignal = registerSignal("splitTaskComplete");
    taskTimeSignal = registerSignal("taskTime");
    rewardSignal = registerSignal("reward");
    completedDelayToleranceSignal = registerSignal("completedDelayTolerance");
    delayToToleranceMultipleSignal = registerSignal("delayToToleranceMultiple");
    remainedTimeRatioSignal = registerSignal("remainedTimeRatio");
}

void FinishedTaskCollector::handleMessage(omnetpp::cMessage *msg)
{
    Task* task = omnetpp::check_and_cast<Task*>(msg);

    if (task->getTotalSubTaskCount() == task->getSubTaskVec().size()) {
        emitSignal(task);
    } else {
        int64_t taskId = task->getTaskId();
        std::unordered_map<int64_t, std::pair<std::vector<Task*>, int>>::iterator it = taskMap.find(taskId);
        if (it != taskMap.end()) {
            it->second.first.push_back(task);
            it->second.second += task->getSubTaskVec().size();
        } else {
            taskMap.insert({taskId, {std::vector<Task*>(), task->getSubTaskVec().size()}});
            it = taskMap.find(taskId);
            it->second.first.push_back(task);
        }

        if (task->getTotalSubTaskCount() == it->second.second) {
            emitSignal(it->second.first);
            taskMap.erase(taskId);
        }
    }

}


void FinishedTaskCollector::emitSignal(Task *task) {
    ASSERT(task->getSubTaskVec().size() <= task->getTotalSubTaskCount());

    emit(isCompletedSignal, task->isCompleted());
    if (task->isCompleted()) {
        emit(taskTimeSignal, (task->getFinishedTime() - task->getCreationTime()).dbl());
        emit(rewardSignal, task->getReward());
        emit(completedDelayToleranceSignal, task->getDelayTolerance());
        emit(delayToToleranceMultipleSignal, task->getDelayTolerance() /
                (task->getFinishedTime() - task->getCreationTime()).dbl());
        emit(remainedTimeRatioSignal, (task->getDelayTolerance() - (task->getFinishedTime() - task->getCreationTime()).dbl()) /
                task->getDelayTolerance());
    }
    cancelAndDelete(task);
}


void FinishedTaskCollector::emitSignal(std::vector<Task*>& subTaskVector) {
    Task *task;
    double totalTaskSize = 0;
    double totalRequiredCycle = 0;
    bool isCompleted = true;
    bool containCompletedTask = false;
    bool containNotCompletedTask = false;
    int subTaskCount = 0;
    omnetpp::simtime_t creationTime = subTaskVector[0]->getCreationTime();
    omnetpp::simtime_t finishedTime = subTaskVector[0]->getFinishedTime();
    for (int i = 0; i < subTaskVector.size(); ++i) {
        task = subTaskVector[i];
        totalTaskSize += task->getTaskSize();
        totalRequiredCycle += task->getRequiredCycle();
        isCompleted = isCompleted && task->isCompleted();
        if (finishedTime < subTaskVector[i]->getFinishedTime())
            finishedTime = subTaskVector[i]->getFinishedTime();

        if (task->isCompleted())
            containCompletedTask = true;
        else
            containNotCompletedTask = true;
        subTaskCount += task->getSubTaskVec().size();
    }

    ASSERT(subTaskCount <= task->getTotalSubTaskCount());

    emit(isCompletedSignal, isCompleted);
    emit(splitTaskPartialCompleteSignal, (containCompletedTask && containNotCompletedTask));
    emit(splitTaskCompleteSignal, isCompleted);
    if (isCompleted) {
        emit(taskTimeSignal, (finishedTime - creationTime).dbl());
        emit(rewardSignal, task->getReward());
        emit(completedDelayToleranceSignal, task->getDelayTolerance());
        emit(delayToToleranceMultipleSignal, task->getDelayTolerance() /
                (finishedTime - creationTime).dbl());
        emit(remainedTimeRatioSignal, (task->getDelayTolerance() - (finishedTime - creationTime).dbl()) /
                task->getDelayTolerance());
    }
    for (int i = 0; i < subTaskVector.size(); ++i) {
        task = subTaskVector[i];
        cancelAndDelete(task);
    }
}

void FinishedTaskCollector::finish()
{
    std::vector<Task*> curSubTaskVec;
    for (auto& it: taskMap) {
        curSubTaskVec = it.second.first;
        for (auto *task: curSubTaskVec)
            cancelAndDelete(task);
    }
}
