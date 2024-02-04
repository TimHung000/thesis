/*
 * TaskCollection.cc
 *
 *  Created on: Feb 4, 2024
 *      Author: tim
 */

#include "FinishedTaskCollector.h"

#include "Task_m.h"

Define_Module(FinishedTaskCollector);

void FinishedTaskCollector::initialize()
{
    lifeTimeSignal = registerSignal("lifeTime");
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
    emit(lifeTimeSignal, omnetpp::simTime() - task->getCreationTime());
    emit(totalWaitingTimeSignal, task->getTotalWaitingTime());
    emit(totalProcessingTimeSignal, task->getTotalProcessingTime());
    emit(totalPropagationTimeSignal, task->getTotalPropagationTime());
    emit(deadlineSignal, task->getDeadline());
    emit(taskSizeSignal, task->getTaskSize());
    emit(cpuCyclesSignal, task->getCpuCycles());
    emit(processedCyclesSignal, task->getProcessedCycles());
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
