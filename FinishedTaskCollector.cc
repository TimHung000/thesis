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

Define_Module(FinishedTaskCollector);

void FinishedTaskCollector::initialize()
{
    creationTimeSignal = registerSignal("creationTime");
    totalWaitingTimeSignal = registerSignal("totalWaitingTime");
    totalProcessingTimeSignal = registerSignal("totalProcessingTime");
    totalPropagationTimeSignal = registerSignal("totalPropagationTime");
    deadlineSignal = registerSignal("deadline");
    taskSizeSignal = registerSignal("taskSize");
    cpuCyclesSignal = registerSignal("cpuCycles");
    processedCyclesSignal = registerSignal("processedCycles");
    arrivingServerSignal = registerSignal("arrivingServer");
    runningServerSignal = registerSignal("runningServer");
    hopCountSignal = registerSignal("hopCount");
    isCompletedSignal = registerSignal("isCompleted");
}

void FinishedTaskCollector::handleMessage(omnetpp::cMessage *msg)
{
    Task* task = omnetpp::check_and_cast<Task*>(msg);

    // gather statistics
    emit(creationTimeSignal, task->getCreationTime());
    emit(totalWaitingTimeSignal, task->getTotalWaitingTime());
    emit(totalProcessingTimeSignal, task->getTotalProcessingTime());
    emit(totalPropagationTimeSignal, task->getTotalPropagationTime());
    emit(deadlineSignal, task->getDeadline());
    emit(taskSizeSignal, task->getTaskSize());
    emit(cpuCyclesSignal, task->getRequiredCycle());
    emit(processedCyclesSignal, task->getProcessedCycle());
    emit(arrivingServerSignal, task->getArrivingServer());
    emit(runningServerSignal, task->getRunningServer());
    emit(hopCountSignal, task->getHopCount());
    emit(isCompletedSignal, task->isCompleted());
    delete msg;
}

void FinishedTaskCollector::finish()
{
    // TODO missing scalar statistics
}
