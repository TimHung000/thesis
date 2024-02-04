/*
 * Dispatcher.cc
 *
 *  Created on: Jan 29, 2024
 *      Author: tim
 */

#include <vector>
#include <random>
#include <string>
#include <unordered_map>
#include "Dispatcher.h"
#include "Processor.h"
#include "InfoHandler.h"
#include "Task_m.h"
#include "Info_m.h"

Define_Module(Dispatcher);

Dispatcher::~Dispatcher()
{

}

void Dispatcher::initialize()
{
    int outputGateSize = gateSize("otherEdgeServerOut");
    for (int i = 0; i < outputGateSize; ++i){
        omnetpp::cGate* curGate = gate("otherEdgeServerOut", i);
        connectedGates.push_back(curGate);
        int neighborServerIndex = curGate->getNextGate()->getNextGate()->getOwnerModule()->getIndex();
        connectedServerIndices.push_back(neighborServerIndex);
    }

    memoryThreshold = par("memoryThreshold");
    maximumHop = par("maximumHop");
}

void Dispatcher::handleMessage(omnetpp::cMessage *msg)
{

    Processor *processorModule = omnetpp::check_and_cast<Processor*>(getParentModule()->getSubmodule("processor"));
    Info *thisServerInfo = processorModule->createEdgeServerInfoMsg();
    int serverIdx = thisServerInfo->getServerIdx();
    double serverFrequency = thisServerInfo->getServerFrequency();
    double serverCapacity = thisServerInfo->getServerCapacity();
    int taskCount = thisServerInfo->getTaskCount();
    double totalRequiredCycle = thisServerInfo->getTotalRequiredCycle();
    double totalMemoryConsumed = thisServerInfo->getTotalMemoryConsumed();
    cancelAndDelete(thisServerInfo);

    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);
    incomingTask->setHopCount(incomingTask->getHopCount() + 1);
    intVector& hopPathForUpdate = incomingTask->getHopPathForUpdate();
    hopPathForUpdate.push_back(serverIdx);

    /***********************************************
     * Random Walk based Load Balancing Algorithm  *
     ***********************************************/
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDeadline() * 1e-3) {
        send(incomingTask, "taskFinishedOut");
        return;
    } else if (incomingTask->getHopCount() >= maximumHop || totalMemoryConsumed <= serverCapacity * memoryThreshold / 100) {
        send(incomingTask, "myServerOut");
        return;
    } else {
        int randInt = intrand(gateSize("otherEdgeServerOut"));
        send(incomingTask, gate("otherEdgeServerOut", randInt));
        return;
    }

    /***********************************************
     * Distributed Dynamic Offloading Mechanism    *
     ***********************************************/
//    double totalDelay = (totalRequiredCycle + incomingTask->getCpuCycles()) / serverFrequency;
//    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDeadline() * 1e-3) {
//        send(incomingTask, "taskFinishedOut");
//        return;
//    } else if (omnetpp::simTime() + totalDelay <= incomingTask->getCreationTime() + incomingTask->getDeadline() * 1e-3
//            && totalMemoryConsumed + incomingTask->getTaskSize() <= serverCapacity) {
//        send(incomingTask, "myServerOut");
//        return;
//    }
//
//    InfoHandler *infoHandlerModule = omnetpp::check_and_cast<InfoHandler*>(getParentModule()->getSubmodule("infoHandler"));
//    std::unordered_map<int, Info*> otherServerInfoMap = infoHandlerModule->getEdgeServerStatus();
//    Info* curInfo = otherServerInfoMap[connectedServerIndices[0]];
//    int bestIdx = 0;
//    double bestPredictedDelay = (curInfo->getTotalRequiredCycle() + incomingTask->getCpuCycles()) / curInfo->getServerFrequency();
//    bool bestCanPlaceTask = curInfo->getTotalMemoryConsumed() + incomingTask->getTaskSize() <= curInfo->getServerCapacity();
//    double curPredictedDelay;
//    bool curCanPlaceTask;
//    for (int i = 1; i < connectedServerIndices.size(); ++i) {
//        curInfo = otherServerInfoMap[connectedServerIndices[i]];
//        curPredictedDelay = (curInfo->getTotalRequiredCycle() + incomingTask->getCpuCycles()) / curInfo->getServerFrequency();
//        curCanPlaceTask = curInfo->getTotalMemoryConsumed() + incomingTask->getTaskSize() <= curInfo->getServerCapacity();
//        if ((curCanPlaceTask && curPredictedDelay < bestPredictedDelay)
//             || (!bestCanPlaceTask && curCanPlaceTask)
//             || (!bestCanPlaceTask && curPredictedDelay < bestPredictedDelay)) {
//            bestIdx = i;
//            bestPredictedDelay = curPredictedDelay;
//            curCanPlaceTask = curCanPlaceTask;
//        }
//    }
//    send(incomingTask, connectedGates[bestIdx]);
//    return;
    /***********************************************
     *           Proposed Method                   *
     ***********************************************/
}

void Dispatcher::refreshDisplay() const
{
    // Convert taskName to std::string
    std::string statusTag = "execute";

    // Set the tag argument in the display string
    getDisplayString().setTagArg("processor", 0, statusTag.c_str());
}

void Dispatcher::finish()
{
}
