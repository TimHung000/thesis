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
#include <algorithm>
#include "Dispatcher.h"
#include "Processor.h"
#include "InfoHandler.h"
#include "Task_m.h"
#include "Info_m.h"
#include "SubTask.h"
#include <queue>

Define_Module(Dispatcher);

Dispatcher::~Dispatcher()
{

}

void Dispatcher::initialize()
{
    int outputGateSize = getParentModule()->gateSize("ports$o");
    for (int i = 0; i < outputGateSize; ++i){
        int neighborServerIndex = getParentModule()->gate("ports$o", i)->getNextGate()->getOwnerModule()->getIndex();
        neighborSevers.push_back(neighborServerIndex);
    }
    serverId = getParentModule()->getIndex();
    memoryThreshold = par("memoryThreshold");
    maximumHop = par("maximumHop");
    dispatchingAlgo = par("dispatchingAlgo").stdstringValue();

    totalTaskDrop = 0;
    totalTaskDropSignal = registerSignal("totalTaskDrop");
    reportMsg = new omnetpp::cMessage("reportMsg");
    scheduleAt(0, reportMsg);
}

void Dispatcher::handleMessage(omnetpp::cMessage *msg)
{
    if (msg == reportMsg) {

        emit(totalTaskDropSignal, totalTaskDrop);
        totalTaskDrop = 0;
        scheduleAfter(1, reportMsg);
        return;
    }

    if (dispatchingAlgo == "Random") {
        /***********************************************
         * Random Walk based Load Balancing Algorithm  *
         ***********************************************/
        EV << "dispatcher running Random algorithm" << omnetpp::endl;

        Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

        // task exceed the deadline
        if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDeadline()) {
            incomingTask->setRunningServer(serverId);
            intVector& hopPathForUpdate = incomingTask->getHopPathForUpdate();
            hopPathForUpdate.push_back(serverId);
            incomingTask->setFromDispatcher(true);
            totalTaskDrop += 1;
            send(incomingTask, "taskFinishedOut");
            return;
        }

        // if the current server can accommodate the task then send to the processor
        // otherwise, randomly offload to neighbor server
        Processor *processorModule = omnetpp::check_and_cast<Processor*>(getParentModule()->getSubmodule("processor"));
        Info *thisServerInfo = processorModule->createEdgeServerInfoMsg();
        EV << "test" << omnetpp::endl;
        EV << "added size: " << thisServerInfo->getTotalMemoryConsumed() + incomingTask->getTaskSize() << omnetpp::endl;
        EV << "capacity: " << thisServerInfo->getServerCapacity() << omnetpp::endl;
        EV << "true or false: " << (thisServerInfo->getTotalMemoryConsumed() + incomingTask->getTaskSize() <= thisServerInfo->getServerCapacity()) << omnetpp::endl;
        if (thisServerInfo->getServerCapacity() - thisServerInfo->getTotalMemoryConsumed() >= incomingTask->getTaskSize() &&
                omnetpp::simTime() + (thisServerInfo->getTotalRequiredCycle() + incomingTask->getRequiredCycle()) / thisServerInfo->getServerFrequency()
                       <= incomingTask->getCreationTime() + incomingTask->getDeadline()) {
            incomingTask->setRunningServer(serverId);
            intVector& hopPathForUpdate = incomingTask->getHopPathForUpdate();
            hopPathForUpdate.push_back(serverId);
            send(incomingTask, "localProcessOut");
        } else {
            int randInt = intrand(neighborSevers.size());
            incomingTask->setDestinationServer(neighborSevers[randInt]);
            send(incomingTask, "offloadOut");
        }

        cancelAndDelete(thisServerInfo);
        return;
    } else if (dispatchingAlgo == "Greedy") {
        /***********************************************
         * Distributed Dynamic Offloading Mechanism    *
         ***********************************************/
        EV << "dispatcher running Greedy algorithm" << omnetpp::endl;
        Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

        // task exceed the deadline
        if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDeadline()) {
            incomingTask->setRunningServer(serverId);
            intVector& hopPathForUpdate = incomingTask->getHopPathForUpdate();
            hopPathForUpdate.push_back(serverId);
            totalTaskDrop += 1;
            incomingTask->setFromDispatcher(true);
            send(incomingTask, "taskFinishedOut");
            return;
        }

        // get processor info
        Processor *processorModule = omnetpp::check_and_cast<Processor*>(getParentModule()->getSubmodule("processor"));
        Info *thisServerInfo = processorModule->createEdgeServerInfoMsg();

        // if the current server can accommodate the task, then send to the processor
        if (thisServerInfo->getServerCapacity() - thisServerInfo->getTotalMemoryConsumed() >= incomingTask->getTaskSize() &&
                omnetpp::simTime() + (thisServerInfo->getTotalRequiredCycle() + incomingTask->getRequiredCycle()) / thisServerInfo->getServerFrequency()
                <= incomingTask->getCreationTime() + incomingTask->getDeadline()) {

            incomingTask->setRunningServer(serverId);
            intVector& hopPathForUpdate = incomingTask->getHopPathForUpdate();
            hopPathForUpdate.push_back(serverId);
            send(incomingTask, "localProcessOut");
        } else {
            // find best neighbor to offload the task
            // first sort the neighbor server based on running time of all tasks
            // then choose the first one that can accommodate the task

//            InfoHandler *infoHandlerModule = omnetpp::check_and_cast<InfoHandler*>(getParentModule()->getSubmodule("infoHandler"));
//            std::unordered_map<int, Info*> otherServerInfoMap = infoHandlerModule->getEdgeServerStatus();
//            for (const auto& entry: otherServerInfoMap) {
//                infoVec.push_back(entry.second);
//                EV << "neighbor has server id " << entry.first << omnetpp::endl;
//            }


            std::vector<Info*> infoVec;
            int outputGateSize = getParentModule()->gateSize("ports$o");
            Processor *neighborProcessorModule;
            Info *neighborServerInfo;
            for (int i = 0; i < outputGateSize; ++i){
                neighborProcessorModule  = omnetpp::check_and_cast<Processor*>(getParentModule()->gate("ports$o", i)->getNextGate()->getOwnerModule()->getSubmodule("processor"));
                neighborServerInfo = neighborProcessorModule->createEdgeServerInfoMsg();
                infoVec.push_back(neighborServerInfo);
            }

            std::sort(infoVec.begin(), infoVec.end(), [](const Info* lhs, const Info* rhs) {
                double lhsRemainedSpace = lhs->getServerCapacity() - lhs->getTotalMemoryConsumed();
                double rhsRemainedSpace = rhs->getServerCapacity() - rhs->getTotalMemoryConsumed();
                return lhsRemainedSpace > rhsRemainedSpace;
            });

            // if all neighbor server is out of memory space, then choose the one with less running time of all tasks
            incomingTask->setDestinationServer(infoVec[0]->getServerIdx());
            send(incomingTask, "offloadOut");
            for (auto& info: infoVec)
                cancelAndDelete(info);


        }

        cancelAndDelete(thisServerInfo);

    } else if (dispatchingAlgo == "Proposed") {
        /***********************************************
         *           Proposed Method                   *
         ***********************************************/
        EV << "dispatcher running Proposed algorithm" << omnetpp::endl;
        Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

        // task exceed deadline
        if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDeadline()) {
            incomingTask->setRunningServer(serverId);
            intVector& hopPathForUpdate = incomingTask->getHopPathForUpdate();
            hopPathForUpdate.push_back(serverId);
            totalTaskDrop += 1;
            incomingTask->setFromDispatcher(true);
            send(incomingTask, "taskFinishedOut");
            return;
        }

        // this server status
        Processor *processorModule = omnetpp::check_and_cast<Processor*>(getParentModule()->getSubmodule("processor"));
        Info *thisServerInfo = processorModule->createEdgeServerInfoMsg();


        if (thisServerInfo->getServerCapacity() - thisServerInfo->getTotalMemoryConsumed() >= incomingTask->getTaskSize() &&
                omnetpp::simTime() + (thisServerInfo->getTotalRequiredCycle() + incomingTask->getRequiredCycle()) / thisServerInfo->getServerFrequency()
                <= incomingTask->getCreationTime() + incomingTask->getDeadline()) {
            incomingTask->setRunningServer(serverId);
            intVector& hopPathForUpdate = incomingTask->getHopPathForUpdate();
            hopPathForUpdate.push_back(serverId);
            send(incomingTask, "localProcessOut");
        } else { // whole task cannot be accommodate by server, so split it




//            InfoHandler *infoHandlerModule = omnetpp::check_and_cast<InfoHandler*>(getParentModule()->getSubmodule("infoHandler"));
//            std::unordered_map<int, Info*> otherServerInfoMap = infoHandlerModule->getEdgeServerStatus();
//            for (const auto& entry: otherServerInfoMap) {
//                infoHeap.push(entry.second);
//            }


            subTaskVector& subTaskVec = incomingTask->getSubTaskVecForUpdate();
            // task has multiple subtasks, so sort it by its required cpu cycle. large..small
            std::sort(subTaskVec.begin(), subTaskVec.end(), [](const SubTask *lhs, const SubTask *rhs) {
                return lhs->getSubTaskSize() >= rhs->getSubTaskSize();
            });

            auto compare = [](Info* lhs, Info* rhs) {
                double lhsRemainedSpace = lhs->getServerCapacity() - lhs->getTotalMemoryConsumed();
                double rhsRemainedSpace = rhs->getServerCapacity() - rhs->getTotalMemoryConsumed();
                return lhsRemainedSpace < rhsRemainedSpace;
            };

            std::priority_queue<Info*, std::vector<Info*>, decltype(compare)> infoHeap(compare);

            int outputGateSize = getParentModule()->gateSize("ports$o");
            Processor *neighborProcessorModule;
            Info *neighborServerInfo;
            for (int i = 0; i < outputGateSize; ++i){
                neighborProcessorModule  = omnetpp::check_and_cast<Processor*>(getParentModule()->gate("ports$o", i)->getNextGate()->getOwnerModule()->getSubmodule("processor"));
                neighborServerInfo = neighborProcessorModule->createEdgeServerInfoMsg();
                infoHeap.push(neighborServerInfo);
            }



            if (subTaskVec.size() == 1) {
                incomingTask->setDestinationServer(infoHeap.top()->getServerIdx());
                send(incomingTask, "offloadOut");
            } else {
                int curSubTaskId = 0;
                double totalSubTaskSize = 0;
                double totalSubTaskRequiredCPUCycle = 0;
                double thisServerPredictedSpace = thisServerInfo->getServerCapacity() - thisServerInfo->getTotalMemoryConsumed();
                omnetpp::simtime_t thisServerPredictedFinishedTime = omnetpp::simTime() + thisServerInfo->getTotalRequiredCycle() / thisServerInfo->getServerFrequency();

                while (curSubTaskId < subTaskVec.size() &&
                        thisServerPredictedSpace >= subTaskVec[curSubTaskId]->getSubTaskSize() &&
                        thisServerPredictedFinishedTime + subTaskVec[curSubTaskId]->getSubTaskRequiredCPUCycle() / thisServerInfo->getServerFrequency()
                        <= incomingTask->getCreationTime() + incomingTask->getDeadline()) {

                    Task *dupTask = incomingTask->dup();

                    totalSubTaskSize += subTaskVec[curSubTaskId]->getSubTaskSize();
                    dupTask->setTaskSize(subTaskVec[curSubTaskId]->getSubTaskSize());

                    totalSubTaskRequiredCPUCycle += subTaskVec[curSubTaskId]->getSubTaskRequiredCPUCycle();
                    dupTask->setRequiredCycle(subTaskVec[curSubTaskId]->getSubTaskRequiredCPUCycle());

                    std::vector<int> hopPath(incomingTask->getHopPath());
                    hopPath.push_back(serverId);
                    dupTask->setHopPath(hopPath);
                    dupTask->setSubTaskVec({subTaskVec[curSubTaskId]});
                    dupTask->setDestinationServer(serverId);
                    dupTask->setRunningServer(serverId);
                    send(dupTask, "localProcessOut");

                    thisServerPredictedSpace -= subTaskVec[curSubTaskId]->getSubTaskSize();
                    curSubTaskId += 1;
                }

                Info *targetServerInfo;
                while (curSubTaskId < subTaskVec.size()) {
                    targetServerInfo = infoHeap.top();
                    infoHeap.pop();

                    Task *dupTask = incomingTask->dup();

                    totalSubTaskSize += subTaskVec[curSubTaskId]->getSubTaskSize();
                    dupTask->setTaskSize(subTaskVec[curSubTaskId]->getSubTaskSize());

                    totalSubTaskRequiredCPUCycle += subTaskVec[curSubTaskId]->getSubTaskRequiredCPUCycle();
                    dupTask->setRequiredCycle(subTaskVec[curSubTaskId]->getSubTaskRequiredCPUCycle());

                    std::vector<int> hopPath(incomingTask->getHopPath());
                    dupTask->setHopPath(hopPath);
                    dupTask->setSubTaskVec({subTaskVec[curSubTaskId]});
                    dupTask->setDestinationServer(targetServerInfo->getServerIdx());
                    send(dupTask, "offloadOut");

                    targetServerInfo->setTotalMemoryConsumed(targetServerInfo->getTotalMemoryConsumed() + subTaskVec[curSubTaskId]->getSubTaskSize());
                    infoHeap.push(targetServerInfo);

                    curSubTaskId += 1;
                }

                ASSERT(totalSubTaskSize == incomingTask->getTaskSize());
                ASSERT(totalSubTaskRequiredCPUCycle == incomingTask->getRequiredCycle());

                cancelAndDelete(incomingTask);    // incoming task is divided into multiple subtask

            }
            Info *info;
            while (!infoHeap.empty()) {
                info = infoHeap.top();
                infoHeap.pop();
                cancelAndDelete(info);
            }

        }
        cancelAndDelete(thisServerInfo);
    }
}

void Dispatcher::refreshDisplay() const
{

}

void Dispatcher::finish()
{
}
