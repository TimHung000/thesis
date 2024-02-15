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
    algorithm = par("algorithm").stdstringValue();
    EV << "server " << serverId << "use " << algorithm << "algorithm" << omnetpp::endl;
}

void Dispatcher::handleMessage(omnetpp::cMessage *msg)
{
    if (algorithm == "Random") {
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
            send(incomingTask, "taskFinishedOut");
            return;
        }

        // if the current server can accommodate the task then send to the processor
        // otherwise, randomly offload to neighbor server
        Processor *processorModule = omnetpp::check_and_cast<Processor*>(getParentModule()->getSubmodule("processor"));
        Info *thisServerInfo = processorModule->createEdgeServerInfoMsg();
        if (thisServerInfo->getTotalMemoryConsumed() + incomingTask->getTaskSize()
                    <= thisServerInfo->getServerCapacity()) {
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
    } else if (algorithm == "Greedy") {
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
            send(incomingTask, "taskFinishedOut");
            return;
        }

        // get processor info
        Processor *processorModule = omnetpp::check_and_cast<Processor*>(getParentModule()->getSubmodule("processor"));
        Info *thisServerInfo = processorModule->createEdgeServerInfoMsg();

        // if the current server can accommodate the task, then send to the processor
        if (thisServerInfo->getTotalMemoryConsumed() + incomingTask->getTaskSize()
                    <= thisServerInfo->getServerCapacity()) {
            incomingTask->setRunningServer(serverId);
            intVector& hopPathForUpdate = incomingTask->getHopPathForUpdate();
            hopPathForUpdate.push_back(serverId);
            send(incomingTask, "localProcessOut");

        } else {
            // find best neighbor to offload the task
            // first sort the neighbor server based on running time of all tasks
            // then choose the first one that can accommodate the task
            InfoHandler *infoHandlerModule = omnetpp::check_and_cast<InfoHandler*>(getParentModule()->getSubmodule("infoHandler"));
            std::unordered_map<int, Info*> otherServerInfoMap = infoHandlerModule->getEdgeServerStatus();
            std::vector<Info*> infoVec;

            for (const auto& entry: otherServerInfoMap) {
                infoVec.push_back(entry.second);
                EV << "neighbor has server id " << entry.first << omnetpp::endl;
            }

            std::sort(infoVec.begin(), infoVec.end(), [](const Info* lhs, const Info* rhs) {
                double lhsRemainedSpace = lhs->getServerCapacity() - lhs->getTotalMemoryConsumed();
                double rhsRemainedSpace = rhs->getServerCapacity() - rhs->getTotalMemoryConsumed();
                return lhsRemainedSpace > rhsRemainedSpace;
            });

            // if all neighbor server is out of memory space, then choose the one with less running time of all tasks
            incomingTask->setDestinationServer(infoVec[0]->getServerIdx());
            send(incomingTask, "offloadOut");
//            int randInt = intrand(neighborSevers.size());
//            incomingTask->setDestinationServer(neighborSevers[randInt]);
//            send(incomingTask, "offloadOut");
        }

        cancelAndDelete(thisServerInfo);

    } else if (algorithm == "Proposed") {
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
            send(incomingTask, "taskFinishedOut");
            return;
        }


        // collect all the server info within maxHop include this server

        Processor *processorModule = omnetpp::check_and_cast<Processor*>(getParentModule()->getSubmodule("processor"));
        Info *thisServerInfo = processorModule->createEdgeServerInfoMsg();
        InfoHandler *infoHandlerModule = omnetpp::check_and_cast<InfoHandler*>(getParentModule()->getSubmodule("infoHandler"));
        std::unordered_map<int, Info*> otherServerInfoMap = infoHandlerModule->getEdgeServerStatus();

        std::vector<Info*> infoVec;
        infoVec.push_back(thisServerInfo);
        for (const auto& entry: otherServerInfoMap)
            infoVec.push_back(entry.second);

        // sort all the server info based on its running time of all tasks. short to long
        std::sort(infoVec.begin(), infoVec.end(), [](const Info* lhs, const Info* rhs) {
            double lhsPredictedDelay = lhs->getTotalRequiredCycle() / lhs->getServerFrequency()
                    + static_cast<double>(lhs->getHopCount()) * 5 * 1e-3;
            double rhsPredictedDelay = rhs->getTotalRequiredCycle() / rhs->getServerFrequency()
                    + static_cast<double>(rhs->getHopCount()) * 5 * 1e-3;

            return lhsPredictedDelay < rhsPredictedDelay;
        });

        // get the sub task list of this main task
        subTaskVector& subTaskVec = incomingTask->getSubTaskVecForUpdate();
        EV << "task size is : " << incomingTask->getTaskSize() << omnetpp::endl;
        for (int i = 0; i < subTaskVec.size(); ++i) {
            EV << "subtask " << i << " size is : " << subTaskVec[i]->getSubTaskSize() << omnetpp::endl;
        }

        // if this task is already split or cannot split, then find the best server and offload it
        if (subTaskVec.size() == 1) {
            EV << "subtask count is 1"  << omnetpp::endl;
            int bestIdx = 0;
            for (int i = 0; i < infoVec.size(); ++i) {
                if (infoVec[i]->getTotalMemoryConsumed() + incomingTask->getTaskSize()
                        <= infoVec[i]->getServerCapacity()) {
                    bestIdx = i;
                    break;
                }
            }

            int destinationServerId = infoVec[bestIdx]->getServerIdx();
            incomingTask->setDestinationServer(destinationServerId);
            if (destinationServerId == serverId)
                send(incomingTask, "localProcessOut");
            else
                send(incomingTask, "offloadOut");

            cancelAndDelete(thisServerInfo);
            return;
        }


        // task has multiple subtasks, so sort it by its required cpu cycle. large..small
        std::sort(subTaskVec.begin(), subTaskVec.end(), [](const SubTask *lhs, const SubTask *rhs) {
            return lhs->getSubTaskRequiredCPUCycle() >= rhs->getSubTaskRequiredCPUCycle();
        });


        // assign the sub task to the currently best server sequentially
        // the best server is the first entry of info vector
        // as we assign the task to it, it's running time will increase.
        // when it's running time bigger than the second best, it will go to the next
        int curSubTaskIdx = 0;
        Info* curServerInfo;
        Info* nextServerInfo;
        double curServerPredictedDelay;
        double nextServerPredictedDelay;
        int destinationServerId;
        for (int i = 0; i < infoVec.size() - 1; ++i) {
            curServerInfo = infoVec[i];
            nextServerInfo = infoVec[i+1];
            curServerPredictedDelay = curServerInfo->getTotalRequiredCycle() / curServerInfo->getServerFrequency()
                    + static_cast<double>(curServerInfo->getHopCount()) * 5 * 1e-3;
            nextServerPredictedDelay = nextServerInfo->getTotalRequiredCycle() / nextServerInfo->getServerFrequency()
                    + static_cast<double>(nextServerInfo->getHopCount()) * 5 * 1e-3;
            while(curSubTaskIdx < subTaskVec.size() && curServerPredictedDelay < nextServerPredictedDelay) {
                // decide to send task to cur server
                // create a new task that represent the subTask
                // the taskId of subTask will be the same as main Task
                Task *dupTask = incomingTask->dup();
                dupTask->setTaskSize(subTaskVec[curSubTaskIdx]->getSubTaskSize());
                dupTask->setRequiredCycle(subTaskVec[curSubTaskIdx]->getSubTaskRequiredCPUCycle());
                std::vector<int> hopPath(incomingTask->getHopPath());
                dupTask->setHopPath(hopPath);
                dupTask->setSubTaskVec({subTaskVec[curSubTaskIdx]});
                destinationServerId = curServerInfo->getServerIdx();
                dupTask->setDestinationServer(destinationServerId);
                EV << dupTask->getName() << " - " << curSubTaskIdx << " is offload to " <<  destinationServerId << omnetpp::endl;
                if (destinationServerId == serverId) {
                    dupTask->setRunningServer(serverId);
                    intVector& hopPathForUpdate = dupTask->getHopPathForUpdate();
                    hopPathForUpdate.push_back(serverId);
                    send(dupTask, "localProcessOut");
                } else {
                    send(dupTask, "offloadOut");
                }
                // update the current best server until it is not the best then go to second
                curServerPredictedDelay += subTaskVec[curSubTaskIdx]->getSubTaskRequiredCPUCycle() / curServerInfo->getServerFrequency();
                // decide the next subTask
                curSubTaskIdx += 1;
            }

            // already offload all the subtask
            if (curSubTaskIdx >= subTaskVec.size())
                break;
        }

        cancelAndDelete(incomingTask);    // incoming task is divided into multiple subtask
        cancelAndDelete(thisServerInfo);
        return;
    } else {
        ASSERT(false);
        return;
    }
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
