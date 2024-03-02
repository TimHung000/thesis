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
//    taskIdSignal = registerSignal("taskId");
//    creationTimeSignal = registerSignal("creationTime");
//    totalWaitingTimeSignal = registerSignal("totalWaitingTime");
//    totalProcessingTimeSignal = registerSignal("totalProcessingTime");
//    totalPropagationTimeSignal = registerSignal("totalPropagationTime");
//    deadlineSignal = registerSignal("deadline");
//    finishedTimeSignal = registerSignal("finishedTime");
//    taskSizeSignal = registerSignal("taskSize");
//    requiredCycleSignal = registerSignal("requiredCycle");
//    processedCyclesSignal = registerSignal("processedCycles");
//    arrivingServerSignal = registerSignal("arrivingServer");
//    runningServerSignal = registerSignal("runningServer");
//    hopCountSignal = registerSignal("hopCount");
    isCompletedSignal = registerSignal("isCompleted");
//    totalSubTaskCountSignal = registerSignal("totalSubTaskCount");
    splitTaskPartialCompleteSignal = registerSignal("splitTaskPartialComplete");
    splitTaskCompleteSignal = registerSignal("splitTaskComplete");
    totalTimeSignal = registerSignal("totalTime");
    totalRewardSignal = registerSignal("totalReward");
}

void FinishedTaskCollector::handleMessage(omnetpp::cMessage *msg)
{
    Task* task = omnetpp::check_and_cast<Task*>(msg);

    // never split
    if (task->getTotalSubTaskCount() == task->getSubTaskVec().size()) {
//        EV << "task not splited" << omnetpp::endl;
        emitSignal(task);
    } else {
//        EV << "task has splited" << omnetpp::endl;

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

//    emit(taskIdSignal, static_cast<double>(task->getTaskId()));
//    emit(creationTimeSignal, task->getCreationTime());
//    emit(totalWaitingTimeSignal, task->getTotalWaitingTime());
//    emit(totalProcessingTimeSignal, task->getTotalProcessingTime());
//    emit(totalPropagationTimeSignal, task->getTotalPropagationTime());
//    emit(deadlineSignal, task->getDeadline());
//    emit(finishedTimeSignal, task->getFinishedTime());
//    emit(taskSizeSignal, task->getTaskSize());
//    emit(requiredCycleSignal, task->getRequiredCycle());
//    emit(processedCyclesSignal, task->getProcessedCycle());
//    emit(arrivingServerSignal, task->getArrivingServer());
//    emit(runningServerSignal, task->getRunningServer());
//    emit(hopCountSignal, task->getHopCount());
    emit(isCompletedSignal, task->isCompleted());
//    emit(totalSubTaskCountSignal, task->getTotalSubTaskCount());
    if (task->isCompleted()) {
        emit(totalTimeSignal, task->getFinishedTime() - task->getCreationTime());
        emit(totalRewardSignal, task->getReward());
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
    // task is the last in the vector
//    emit(taskIdSignal, static_cast<double>(task->getTaskId()));
//    emit(creationTimeSignal, task->getCreationTime());
//    emit(totalWaitingTimeSignal, task->getTotalWaitingTime());
//    emit(totalProcessingTimeSignal, task->getTotalProcessingTime());
//    emit(totalPropagationTimeSignal, task->getTotalPropagationTime());
//    emit(deadlineSignal, task->getDeadline());
//    emit(finishedTimeSignal, task->getFinishedTime());
//    emit(taskSizeSignal, totalTaskSize);
//    emit(requiredCycleSignal, totalRequiredCycle);
//    emit(processedCyclesSignal, task->getProcessedCycle());
//    emit(arrivingServerSignal, task->getArrivingServer());
//    emit(runningServerSignal, task->getRunningServer());
//    emit(hopCountSignal, task->getHopCount());
    emit(isCompletedSignal, isCompleted);
//    emit(totalSubTaskCountSignal, task->getTotalSubTaskCount());
    emit(splitTaskPartialCompleteSignal, (containCompletedTask && containNotCompletedTask));
    emit(splitTaskCompleteSignal, isCompleted);
    if (isCompleted) {
        emit(totalTimeSignal, finishedTime - creationTime);
        emit(totalRewardSignal, task->getReward());
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
