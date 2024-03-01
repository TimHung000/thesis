/*
 * Dispatcher.cc
 *
 *  Created on: Jan 29, 2024
 *      Author: tim
 */

#include "TaskQueue.h"

#include <vector>
#include <random>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "Task_m.h"
#include "SubTask.h"
#include <queue>
#include <limits>

Define_Module(TaskQueue);

TaskQueue::~TaskQueue()
{
    for (std::list<Task*>::iterator it = waitingQueue.begin(); it != waitingQueue.end(); ++it)
        cancelAndDelete(*it);

    for (std::list<Task*>::iterator it = garbageQueue.begin(); it != garbageQueue.end(); ++it)
        cancelAndDelete(*it);

    if (runningTask)
        cancelAndDelete(runningTask);

    cancelAndDelete(serverInfoSignalMsg);
}

void TaskQueue::initialize()
{
    // server basic info
    serverId = getParentModule()->getIndex();
    int outputGateSize = getParentModule()->gateSize("ports$o");
    for (int i = 0; i < outputGateSize; ++i){
        int neighborServerIndex = getParentModule()->gate("ports$o", i)->getNextGate()->getOwnerModule()->getIndex();
        neighborServers.push_back(neighborServerIndex);
    }

    // initialize server capacity
//    double randFrequency = par("minFrequency").doubleValue() +
//            intrand(par("maxFrequency").doubleValue() - par("minFrequency").doubleValue() + 1);
//    serverFrequency = static_cast<double>(randFrequency) * 1e9;

    double randFrequency = par("minFrequency").doubleValue() +
            intrand(par("maxFrequency").doubleValue() - par("minFrequency").doubleValue() + 1);
    serverFrequency = randFrequency;

//    serverMemory = serverFrequency * par("memoryMultiple").doubleValue();

    double randMemory = par("minMemory").doubleValue() +
            intrand(par("maxMemory").doubleValue() - par("minMemory").doubleValue() + 1);
    serverMemory = randMemory;

    // initialize server status
    totalRequiredCycle = 0;
    totalMemoryConsumed = 0;

    // for dispatching algorithm
    dispatchingAlgo = par("dispatchingAlgo").stdstringValue();


    // for scheduling algorithm
    schedulingAlgo = par("schedulingAlgo").stdstringValue();


    // for random algo used
    memoryThreshold = par("memoryThreshold");
    maximumHop = par("maximumHop");

    // for processing task management
    runningTask = nullptr;


    // signal
    serverInfoSignalMsg = new omnetpp::cMessage("serverInfoSignalMsg");
    totalTaskFailed = 0;
    totalTaskCompleted = 0;
    memoryLoadingSignal = registerSignal("memoryLoading");
    CPULoadingSignal = registerSignal("CPULoading");
    totalTaskSignal = registerSignal("totalTask");
    totalTaskCompletedSignal = registerSignal("totalTaskCompleted");
    totalTaskFailedSignal = registerSignal("totalTaskFailed");
    scheduleAt(0, serverInfoSignalMsg);

}

void TaskQueue::handleMessage(omnetpp::cMessage *msg)
{
    if (msg == serverInfoSignalMsg) {
        EV << "serverMemory: " << serverMemory << omnetpp::endl;
        EV << "totalMemoryConsumed: " << totalMemoryConsumed << omnetpp::endl;

        EV << "serverFrequency: " << serverFrequency << omnetpp::endl;
        EV << "totalRequiredCycle: " << serverMemory << omnetpp::endl;

        emit(memoryLoadingSignal, totalMemoryConsumed / serverMemory);
        emit(CPULoadingSignal, totalRequiredCycle / serverFrequency);
        emit(totalTaskSignal, totalTaskFailed + totalTaskCompleted);
        emit(totalTaskCompletedSignal, totalTaskCompleted);
        emit(totalTaskFailedSignal, totalTaskFailed);
        totalTaskFailed = 0;
        totalTaskCompleted = 0;
        scheduleAfter(1, msg);
        return;
    }

    if (msg == runningTask) {
        ASSERT(runningTask != nullptr);
        omnetpp::simtime_t processingTime = omnetpp::simTime() - runningTask->getSendingTime();
        double processedCycle = processingTime.dbl() * serverFrequency;
        runningTask->setTotalProcessingTime(runningTask->getTotalProcessingTime() + processingTime);
        runningTask->setProcessedCycle(runningTask->getProcessedCycle() + processedCycle);
        runningTask->setFinishedTime(omnetpp::simTime());
        runningTask->setIsCompleted(true);
        totalRequiredCycle -= runningTask->getRequiredCycle();
        totalMemoryConsumed -= runningTask->getTaskSize();
        send(runningTask, "taskFinishedOut");
        totalTaskCompleted += 1;
        runningTask = nullptr;
        scheduling();
        return;
    }

    if (dispatchingAlgo == "Random") {
        randomDispatchingAlgo(msg);
    } else if (dispatchingAlgo == "Random2") {
        random2DispatchingAlgo(msg);
    } else if (dispatchingAlgo == "Greedy") {
        greedyDispatchingAlgo(msg);
    } else if (dispatchingAlgo == "Greedy2") {
        greedy2DispatchingAlgo(msg);
    } else if (dispatchingAlgo == "Proposed") {
        proposedDispatchingAlgo(msg);
    } else if (dispatchingAlgo == "Proposed2") {
        proposedDispatchingAlgo2(msg);
    } else if (dispatchingAlgo == "Proposed3") {
        proposedDispatchingAlgo3(msg);
    }

    scheduling();

}

void TaskQueue::randomDispatchingAlgo(omnetpp::cMessage *msg) {
    /***********************************************
     * Random Walk based Load Balancing Algorithm  *
     ***********************************************/

    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed the deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        totalTaskFailed += 1;
        send(incomingTask, "taskFinishedOut");
        return;
    }

    if (serverMemory - totalMemoryConsumed >= incomingTask->getTaskSize()) {
        insertTaskIntoWaitingQueue(incomingTask);
    } else {
        int randInt = intrand(neighborServers.size());
        incomingTask->setDestinationServer(neighborServers[randInt]);
        send(incomingTask, "offloadOut");
    }
}

void TaskQueue::random2DispatchingAlgo(omnetpp::cMessage *msg) {
    /***********************************************
     * Random Walk based Load Balancing Algorithm  *
     ***********************************************/

    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed the deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        totalTaskFailed += 1;
        send(incomingTask, "taskFinishedOut");
        return;
    }

    // if the current server can accommodate the task then send to the processor
    // otherwise, randomly offload to neighbor server
    omnetpp::simtime_t predictedFinishedTime;
    if (runningTask) {
        predictedFinishedTime = runningTask->getArrivalTime() +
                (totalRequiredCycle - runningTask->getRequiredCycle() + incomingTask->getRequiredCycle()) / serverFrequency;
    } else {
        predictedFinishedTime = omnetpp::simTime() + (totalRequiredCycle + incomingTask->getRequiredCycle()) / serverFrequency;
    }

    if (serverMemory - totalMemoryConsumed >= incomingTask->getTaskSize() &&
            predictedFinishedTime <= incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        insertTaskIntoWaitingQueue(incomingTask);
    } else {
        int randInt = intrand(neighborServers.size());
        incomingTask->setDestinationServer(neighborServers[randInt]);
        send(incomingTask, "offloadOut");
    }

}

void TaskQueue::greedyDispatchingAlgo(omnetpp::cMessage *msg) {
    /***********************************************
     * Distributed Dynamic Offloading Mechanism    *
     ***********************************************/
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed the deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        send(incomingTask, "taskFinishedOut");
        totalTaskFailed += 1;
        return;
    }

    if (serverMemory - totalMemoryConsumed >= incomingTask->getTaskSize()) {
        insertTaskIntoWaitingQueue(incomingTask);
    } else {

        // get neighbor server status
        std::vector<ServerStatus*> neighborServerStatusVec;
        for (int i = 0; i < neighborServers.size(); ++i)
            neighborServerStatusVec.push_back(getServerStatus(neighborServers[i]));

        std::sort(neighborServerStatusVec.begin(), neighborServerStatusVec.end(), [](ServerStatus *lhs, ServerStatus *rhs) {
            return lhs->getServerMemory() - lhs->getTotalMemoryConsumed() > rhs->getServerMemory() - rhs->getTotalMemoryConsumed();
        });

        incomingTask->setDestinationServer(neighborServerStatusVec[0]->getServerId());
        send(incomingTask, "offloadOut");

        for (auto& status: neighborServerStatusVec)
            delete status;
    }
}

void TaskQueue::greedy2DispatchingAlgo(omnetpp::cMessage *msg) {
    /***********************************************
     * Distributed Dynamic Offloading Mechanism    *
     ***********************************************/
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed the deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        send(incomingTask, "taskFinishedOut");
        totalTaskFailed += 1;
        return;
    }

    // if the current server can accommodate the task, then send to the processor
    omnetpp::simtime_t predictedFinishedTime;
    if (runningTask) {
        predictedFinishedTime = runningTask->getArrivalTime() +
                (totalRequiredCycle - runningTask->getRequiredCycle() + incomingTask->getRequiredCycle()) / serverFrequency;
    } else {
        predictedFinishedTime = omnetpp::simTime() + (totalRequiredCycle + incomingTask->getRequiredCycle()) / serverFrequency;
    }

    if (serverMemory - totalMemoryConsumed >= incomingTask->getTaskSize() &&
            predictedFinishedTime <= incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        insertTaskIntoWaitingQueue(incomingTask);
    } else {

        // get neighbor server status
        std::vector<ServerStatus*> neighborServerStatus;
        int outputGateSize = getParentModule()->gateSize("ports$o");
        TaskQueue *neighborTaskQueueModule;
        ServerStatus *curNeighborStatus;
        for (int i = 0; i < outputGateSize; ++i){
            neighborTaskQueueModule  = omnetpp::check_and_cast<TaskQueue*>(getParentModule()->gate("ports$o", i)->getNextGate()->getOwnerModule()->getSubmodule("taskQueue"));
            curNeighborStatus = neighborTaskQueueModule->getServerStatus();
            neighborServerStatus.push_back(curNeighborStatus);
        }

        std::sort(neighborServerStatus.begin(), neighborServerStatus.end(), [=](ServerStatus *lhs, ServerStatus *rhs) {
            double lhsPredictedDelay = (lhs->getTotalRequiredCycle() + incomingTask->getRequiredCycle()) / lhs->getServerFrequency();
            double rhsPredictedDelay = (rhs->getTotalRequiredCycle() + incomingTask->getRequiredCycle()) / rhs->getServerFrequency();

            return lhsPredictedDelay < rhsPredictedDelay;
        });

        int bestNeighborServerId = 0;
        for (int i = 0; i < neighborServerStatus.size(); ++i) {
            if (neighborServerStatus[i]->getTotalMemoryConsumed() + incomingTask->getTaskSize() <= neighborServerStatus[i]->getServerMemory()) {
                bestNeighborServerId = i;
                break;
            }
        }

        incomingTask->setDestinationServer(neighborServerStatus[bestNeighborServerId]->getServerId());
        send(incomingTask, "offloadOut");

        for (auto& status: neighborServerStatus)
            delete status;
    }
}

void TaskQueue::proposedDispatchingAlgo(omnetpp::cMessage *msg) {
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        send(incomingTask, "taskFinishedOut");
        totalTaskFailed += 1;
        return;
    }



    omnetpp::simtime_t incomingTaskDeadline = incomingTask->getCreationTime() + incomingTask->getDelayTolerance();

    if (incomingTask->getSubTaskVec().size() != incomingTask->getTotalSubTaskCount()) {
        if (totalMemoryConsumed + incomingTask->getTaskSize() <= serverMemory) {
            insertTaskIntoWaitingQueue(incomingTask);
        } else {
            std::vector<ServerStatus*> neighborServerStatusVec;
            for (int i = 0; i < neighborServers.size(); ++i)
                neighborServerStatusVec.push_back(getServerStatus(neighborServers[i]));
            std::sort(neighborServerStatusVec.begin(), neighborServerStatusVec.end(), [](ServerStatus *lhs, ServerStatus *rhs) {
                return lhs->getTotalRequiredCycle() / lhs->getServerFrequency() < rhs->getTotalRequiredCycle() / rhs->getServerFrequency();
            });

            incomingTask->setDestinationServer(neighborServerStatusVec[0]->getServerId());
            send(incomingTask, "offloadOut");

        }
    } else {
        ServerStatus *thisServerStatus = getServerStatus();
        std::vector<ServerStatus*> neighborServerStatusVec;
        for (int i = 0; i < neighborServers.size(); ++i)
            neighborServerStatusVec.push_back(getServerStatus(neighborServers[i]));

        std::sort(neighborServerStatusVec.begin(), neighborServerStatusVec.end(), [](ServerStatus *lhs, ServerStatus *rhs) {
            return lhs->getTotalRequiredCycle() / lhs->getServerFrequency() < rhs->getTotalRequiredCycle() / rhs->getServerFrequency();
        });

        double totalNeighborFrequency = 0.0;
        double totalNeighborRemainedSpace = 0.0;
        double totalNeighborRequiredCPUCycle = 0.0;
        double totalNeighborFinishedTime = 0.0;
        for (int i = 0; i < neighborServerStatusVec.size(); ++i) {
            totalNeighborFrequency += neighborServerStatusVec[i]->getServerFrequency();
            totalNeighborRemainedSpace += neighborServerStatusVec[i]->getServerMemory() - neighborServerStatusVec[i]->getTotalMemoryConsumed();
            totalNeighborRequiredCPUCycle += neighborServerStatusVec[i]->getTotalRequiredCycle();
            totalNeighborFinishedTime += neighborServerStatusVec[i]->getTotalRequiredCycle() / neighborServerStatusVec[i]->getServerFrequency();
        }


        double avgServerRequiredCPUCycle = (totalNeighborRequiredCPUCycle + thisServerStatus->getTotalRequiredCycle()) / (neighborServers.size() + 1);
        double avgServerFrequency = (totalNeighborFrequency + thisServerStatus->getServerFrequency()) / (neighborServers.size() + 1);
        double avgServerRemainedSpace = (totalNeighborRemainedSpace + (thisServerStatus->getServerMemory() - thisServerStatus->getTotalMemoryConsumed())) / (neighborServers.size() + 1);

        double avgSubTaskRequiredCPUCycle = (incomingTask->getSubTaskVec().size() < neighborServers.size() + 1) ? incomingTask->getRequiredCycle() / incomingTask->getSubTaskVec().size()
                : incomingTask->getRequiredCycle() / (neighborServers.size() + 1);
        double avgSubTaskSize = (incomingTask->getSubTaskVec().size() < neighborServers.size() + 1) ? incomingTask->getTaskSize() / incomingTask->getSubTaskVec().size() :
                incomingTask->getTaskSize() / (neighborServers.size() + 1);



        const double MultipleForEnsuringTaskCanPlace = 1;
        const double predictedTimeInTransmit = 5 * 1e-3;
        bool canPlaceAllSubTask = avgSubTaskSize * MultipleForEnsuringTaskCanPlace <= avgServerRemainedSpace;
        bool canFinishAllSubTask = omnetpp::simTime() + predictedTimeInTransmit + (avgServerRequiredCPUCycle + avgSubTaskRequiredCPUCycle) / avgServerFrequency <= incomingTaskDeadline;
        bool isIncomingTaskSplittable = incomingTask->getSubTaskVec().size() > 1;

        if (isIncomingTaskSplittable && canPlaceAllSubTask && canFinishAllSubTask) {
            std::vector<ServerStatus*> serverStatusVec(neighborServerStatusVec);
            serverStatusVec.push_back(thisServerStatus);
            std::sort(serverStatusVec.begin(), serverStatusVec.end(), [](ServerStatus *lhs,ServerStatus *rhs) {
                return lhs->getTotalRequiredCycle() / lhs->getServerFrequency() < rhs->getTotalRequiredCycle() / rhs->getServerFrequency();
            });

            std::vector<Task*> taskToOffload;
            const subTaskVector& subTaskVec = incomingTask->getSubTaskVec();
            for (int i = 0; i < subTaskVec.size(); ++i)
                taskToOffload.push_back(createSubTask(incomingTask, i));
            cancelAndDelete(incomingTask);

            std::sort(taskToOffload.begin(), taskToOffload.end(), [](const Task *lhs, const Task *rhs) {
                return lhs->getRequiredCycle() > rhs->getRequiredCycle();
            });

            int curServerStatusIdx = 0;
            for (Task *task: taskToOffload) {
                if (serverStatusVec[curServerStatusIdx]->getServerId() == serverId && totalMemoryConsumed + task->getTaskSize() > serverMemory)
                    curServerStatusIdx = (curServerStatusIdx + 1) % serverStatusVec.size();

                if (serverStatusVec[curServerStatusIdx]->getServerId() == serverId) {
                    insertTaskIntoWaitingQueue(task);
                } else {
                    task->setDestinationServer(serverStatusVec[curServerStatusIdx]->getServerId());
                    send(task, "offloadOut");
                }
                curServerStatusIdx = (curServerStatusIdx + 1) % serverStatusVec.size();
            }

        } else {
            std::sort(neighborServerStatusVec.begin(), neighborServerStatusVec.end(), [](ServerStatus *lhs, ServerStatus *rhs) {
                return lhs->getTotalRequiredCycle() / lhs->getServerFrequency() < rhs->getTotalRequiredCycle() / rhs->getServerFrequency();
            });


            bool canThisServerFinishTask = omnetpp::simTime() + (thisServerStatus->getTotalRequiredCycle() + incomingTask->getRequiredCycle()) / thisServerStatus->getServerFrequency()
                    <= incomingTaskDeadline;
            bool canThisServerPlaceTask = thisServerStatus->getTotalMemoryConsumed() + incomingTask->getTaskSize() <= serverMemory;
            bool thisServerFinishedTimeLessThanNeighborServerFinishedTime = thisServerStatus->getTotalRequiredCycle() / thisServerStatus->getServerFrequency() <=
                    totalNeighborRequiredCPUCycle / totalNeighborFrequency + predictedTimeInTransmit;

            if (canThisServerFinishTask && canThisServerPlaceTask && thisServerFinishedTimeLessThanNeighborServerFinishedTime) {
                insertTaskIntoWaitingQueue(incomingTask);
            } else {
                incomingTask->setDestinationServer(neighborServerStatusVec[0]->getServerId());
                send(incomingTask, "offloadOut");
            }
        }
    }
}

void TaskQueue::proposedDispatchingAlgo2(omnetpp::cMessage *msg) {
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);

    // task exceed deadline
    if (omnetpp::simTime() > incomingTask->getCreationTime() + incomingTask->getDelayTolerance()) {
        send(incomingTask, "taskFinishedOut");
        totalTaskFailed += 1;
        return;
    }

    omnetpp::simtime_t incomingTaskDeadline = incomingTask->getCreationTime() + incomingTask->getDelayTolerance();

    if (incomingTask->getSubTaskVec().size() != incomingTask->getTotalSubTaskCount()) {
        if (totalMemoryConsumed + incomingTask->getTaskSize() <= serverMemory) {
            insertTaskIntoWaitingQueue(incomingTask);
        } else {
            std::vector<Task*> offloadTasks = replaceTaskWithSubTask(incomingTask);

            std::vector<ServerStatus*> neighborServerStatusVec;
            for (int i = 0; i < neighborServers.size(); ++i)
                neighborServerStatusVec.push_back(getServerStatus(neighborServers[i]));
            std::sort(neighborServerStatusVec.begin(), neighborServerStatusVec.end(), [](ServerStatus *lhs, ServerStatus *rhs) {
                return lhs->getTotalRequiredCycle() / lhs->getServerFrequency() < rhs->getTotalRequiredCycle() / rhs->getServerFrequency();
            });

            for (Task* task: offloadTasks) {
                task->setDestinationServer(neighborServerStatusVec[0]->getServerId());
                send(task, "offloadOut");
            }
        }
    } else {
        ServerStatus *thisServerStatus = getServerStatus();
        std::vector<ServerStatus*> neighborServerStatusVec;
        for (int i = 0; i < neighborServers.size(); ++i)
            neighborServerStatusVec.push_back(getServerStatus(neighborServers[i]));

        double totalNeighborFrequency = 0.0;
        double totalNeighborRemainedSpace = 0.0;
        double totalNeighborRequiredCPUCycle = 0.0;
        double totalNeighborFinishedTime = 0.0;
        for (int i = 0; i < neighborServerStatusVec.size(); ++i) {
            totalNeighborFrequency += neighborServerStatusVec[i]->getServerFrequency();
            totalNeighborRemainedSpace += neighborServerStatusVec[i]->getServerMemory() - neighborServerStatusVec[i]->getTotalMemoryConsumed();
            totalNeighborRequiredCPUCycle += neighborServerStatusVec[i]->getTotalRequiredCycle();
            totalNeighborFinishedTime += neighborServerStatusVec[i]->getTotalRequiredCycle() / neighborServerStatusVec[i]->getServerFrequency();
        }

        const double predictedTimeInTransmit = 5 * 1e-3;
        double avgServerRequiredCPUCycle = (totalNeighborRequiredCPUCycle + thisServerStatus->getTotalRequiredCycle()) / (neighborServers.size() + 1);
        double avgServerFrequency = (totalNeighborFrequency + thisServerStatus->getServerFrequency()) / (neighborServers.size() + 1);
        double avgServerRemainedSpace = (totalNeighborRemainedSpace + (thisServerStatus->getServerMemory() - thisServerStatus->getTotalMemoryConsumed())) / (neighborServers.size() + 1);

        double avgSubTaskRequiredCPUCycle = (incomingTask->getSubTaskVec().size() < neighborServers.size() + 1) ? incomingTask->getRequiredCycle() / incomingTask->getSubTaskVec().size()
                : incomingTask->getRequiredCycle() / (neighborServers.size() + 1);
        double avgSubTaskSize = (incomingTask->getSubTaskVec().size() < neighborServers.size() + 1) ? incomingTask->getTaskSize() / incomingTask->getSubTaskVec().size() :
                incomingTask->getTaskSize() / (neighborServers.size() + 1);

        const double MultipleForEnsuringTaskCanPlace = 1;
        bool canPlaceAllSubTask = avgSubTaskSize * MultipleForEnsuringTaskCanPlace <= avgServerRemainedSpace;
        bool canFinishAllSubTask = omnetpp::simTime() + predictedTimeInTransmit + (avgServerRequiredCPUCycle + avgSubTaskRequiredCPUCycle) / avgServerFrequency <= incomingTaskDeadline;
        bool isIncomingTaskSplittable = incomingTask->getSubTaskVec().size() > 1;

        if (isIncomingTaskSplittable && canPlaceAllSubTask && canFinishAllSubTask) {

            std::vector<ServerStatus*> serverStatusVec(neighborServerStatusVec);
            serverStatusVec.push_back(thisServerStatus);
            std::sort(serverStatusVec.begin(), serverStatusVec.end(), [](ServerStatus *lhs,ServerStatus *rhs) {
                return lhs->getTotalRequiredCycle() / lhs->getServerFrequency() < rhs->getTotalRequiredCycle() / rhs->getServerFrequency();
            });

            std::vector<Task*> taskToOffload;
            const subTaskVector& subTaskVec = incomingTask->getSubTaskVec();
            for (int i = 0; i < subTaskVec.size(); ++i)
                taskToOffload.push_back(createSubTask(incomingTask, i));
            cancelAndDelete(incomingTask);

            std::sort(taskToOffload.begin(), taskToOffload.end(), [](const Task *lhs, const Task *rhs) {
                return lhs->getRequiredCycle() > rhs->getRequiredCycle();
            });


            int curServerStatusIdx = 0;
            for (Task *task: taskToOffload) {
                if (serverStatusVec[curServerStatusIdx]->getServerId() == serverId && totalMemoryConsumed + task->getTaskSize() > serverMemory)
                    curServerStatusIdx = (curServerStatusIdx + 1) % serverStatusVec.size();

                if (serverStatusVec[curServerStatusIdx]->getServerId() == serverId) {
                    insertTaskIntoWaitingQueue(task);
                } else {
                    task->setDestinationServer(serverStatusVec[curServerStatusIdx]->getServerId());
                    send(task, "offloadOut");
                }
                curServerStatusIdx = (curServerStatusIdx + 1) % serverStatusVec.size();
            }
        } else {
            std::sort(neighborServerStatusVec.begin(), neighborServerStatusVec.end(), [](ServerStatus *lhs, ServerStatus *rhs) {
                return lhs->getTotalRequiredCycle() / lhs->getServerFrequency() < rhs->getTotalRequiredCycle() / rhs->getServerFrequency();
            });


            bool canThisServerFinishTask = omnetpp::simTime() + (thisServerStatus->getTotalRequiredCycle() + incomingTask->getRequiredCycle()) / thisServerStatus->getServerFrequency()
                    <= incomingTaskDeadline;
            bool canThisServerPlaceTask = thisServerStatus->getTotalMemoryConsumed() + incomingTask->getTaskSize() <= serverMemory;
            bool canBestNeighborFinishTask = omnetpp::simTime() + (neighborServerStatusVec[0]->getTotalRequiredCycle() + incomingTask->getRequiredCycle()) / neighborServerStatusVec[0]->getServerFrequency()
                    <= incomingTaskDeadline;
            bool thisServerFinishedTimeLessThanNeighborServerFinishedTime = thisServerStatus->getTotalRequiredCycle() / thisServerStatus->getServerFrequency() <=
                    totalNeighborRequiredCPUCycle / totalNeighborFrequency + predictedTimeInTransmit;

            if (canThisServerFinishTask && canThisServerPlaceTask && thisServerFinishedTimeLessThanNeighborServerFinishedTime) {
                insertTaskIntoWaitingQueue(incomingTask);
            } else if (canThisServerFinishTask && canThisServerPlaceTask &&
                    !thisServerFinishedTimeLessThanNeighborServerFinishedTime && !canBestNeighborFinishTask) {
                insertTaskIntoWaitingQueue(incomingTask);
            } else {
                incomingTask->setDestinationServer(neighborServerStatusVec[0]->getServerId());

                send(incomingTask, "offloadOut");
            }
        }
    }
}

void TaskQueue::proposedDispatchingAlgo3(omnetpp::cMessage *msg) {
}

std::vector<Task*> TaskQueue::replaceTaskWithSubTask(Task *task) {
    std::vector<std::list<Task*>::iterator> tmp;
    std::vector<Task*> res;

    double accuMemory = 0.0;
    double accuCycle = 0.0;

    std::list<Task*>::reverse_iterator rit = waitingQueue.rbegin();
    while(rit != waitingQueue.rend()) {
        if ((*rit)->getSubTaskVec().size() == (*rit)->getTotalSubTaskCount()) {
            if ((*rit)->getTaskId() == 1223)
                EV << (*rit)->getTaskId() << omnetpp::endl;
            tmp.push_back(std::next(rit).base());
            accuMemory += (*rit)->getTaskSize();
            accuCycle += (*rit)->getRequiredCycle();
            if (serverMemory - (totalMemoryConsumed - accuMemory) >= task->getTaskSize())
                break;
        }
        ++rit;
    }

    if (serverMemory - (totalMemoryConsumed - accuMemory) >= task->getTaskSize()) {
        for (auto it: tmp) {
            res.push_back(*it);
            waitingQueue.erase(it);
        }
        totalMemoryConsumed -= accuMemory;
        totalRequiredCycle -= accuCycle;
        insertTaskIntoWaitingQueue(task);
    } else {

        res.push_back(task);
    }
    return res;
}


void TaskQueue::scheduling() {
    if (runningTask)
        return;

    // because it's FIFO so, its not sorted by deadline, we need to remove the task in the back
    if (schedulingAlgo == "FIFO") {
        std::list<Task*>::iterator it = waitingQueue.begin();
        while (it != waitingQueue.end()) {
            if (omnetpp::simTime() > (*it)->getCreationTime() + (*it)->getDelayTolerance()) {
                totalRequiredCycle -= (*it)->getRequiredCycle();
                totalMemoryConsumed -= (*it)->getTaskSize();
                send((*it), "taskFinishedOut");
                totalTaskFailed += 1;
                it = waitingQueue.erase(it);
            } else
                ++it;
        }
    }


    // remove tasks that exceed deadline to task collector
    std::list<Task*>::iterator it = garbageQueue.begin();
    while (it != garbageQueue.end()) {
        if (omnetpp::simTime() > (*it)->getCreationTime() + (*it)->getDelayTolerance()) {
            totalRequiredCycle -= (*it)->getRequiredCycle();
            totalMemoryConsumed -= (*it)->getTaskSize();
            send((*it), "taskFinishedOut");
            totalTaskFailed += 1;
            it = garbageQueue.erase(it);
        } else
            ++it;
    }


    // garbage queue is used to collect the task not yet exceed the deadline, but can't be processed in time by the server
    // move tasks that will exceed the deadline to garbage queue
    it = waitingQueue.begin();
    while (it != waitingQueue.end() &&
            omnetpp::simTime() + (*it)->getRequiredCycle() / serverFrequency >
                (*it)->getCreationTime() + (*it)->getDelayTolerance()) {

        garbageQueue.push_back(*it);
        ++it;
        waitingQueue.pop_front();
    }

    if (it != waitingQueue.end()) {
        runningTask = *it;
        waitingQueue.pop_front();
        double runningTime = runningTask->getRequiredCycle() / serverFrequency;
        runningTask->setRunningServer(serverId);
        scheduleAt(omnetpp::simTime() + runningTime, runningTask);
    }
}



void TaskQueue::insertTaskIntoWaitingQueue(Task *task) {

    if (schedulingAlgo == "FIFO") {
        waitingQueue.push_back(task);
        totalRequiredCycle += task->getRequiredCycle();
        totalMemoryConsumed += task->getTaskSize();
    } else if (schedulingAlgo == "EDF") {
        std::list<Task*>::reverse_iterator rit = waitingQueue.rbegin();
        omnetpp::simtime_t taskDeadline = task->getCreationTime() + task->getDelayTolerance();
        while(rit != waitingQueue.rend() && (*rit)->getCreationTime() + (*rit)->getDelayTolerance() > taskDeadline)
            ++rit;

        waitingQueue.insert(rit.base(), task);
        totalRequiredCycle += task->getRequiredCycle();
        totalMemoryConsumed += task->getTaskSize();
    } else if (schedulingAlgo == "EDMPF") {   // Earliest Deadline Minus Processing time First
        std::list<Task*>::reverse_iterator rit = waitingQueue.rbegin();
        omnetpp::simtime_t taskMaxWaitTime = task->getCreationTime() + task->getDelayTolerance()
                - omnetpp::simTime() - task->getTaskSize() / serverFrequency;
        while(rit != waitingQueue.rend() && (*rit)->getCreationTime() + (*rit)->getDelayTolerance()
                - omnetpp::simTime() - (*rit)->getRequiredCycle() / serverFrequency > taskMaxWaitTime)
            ++rit;
        waitingQueue.insert(rit.base(), task);
        totalRequiredCycle += task->getRequiredCycle();
        totalMemoryConsumed += task->getTaskSize();
    }

}


ServerStatus *TaskQueue::getServerStatus() {
    Enter_Method("getServerStatus()");
    double requiredCycle = totalRequiredCycle;
    if (runningTask) {
        requiredCycle = totalRequiredCycle - (omnetpp::simTime() - runningTask->getSendingTime()).dbl() * serverFrequency;
    }
    ServerStatus *serverStatus = new ServerStatus(serverId, serverFrequency, serverMemory,
            requiredCycle, totalMemoryConsumed);

    return serverStatus;
}

ServerStatus *TaskQueue::getServerStatus(int serverId) {
    TaskQueue *taskQueueModule = omnetpp::check_and_cast<TaskQueue*>(getSystemModule()->getSubmodule("edgeServer", serverId)->getSubmodule("taskQueue"));
    ServerStatus *serverStatus = taskQueueModule->getServerStatus();
    return serverStatus;
}

Task *TaskQueue::createSubTask(Task *task, int subTaskIdx) {
    Task *dupTask = task->dup();

    subTaskVector& subTaskVec = task->getSubTaskVecForUpdate();

    dupTask->setTaskSize(subTaskVec[subTaskIdx]->getSubTaskSize());

    dupTask->setRequiredCycle(subTaskVec[subTaskIdx]->getSubTaskRequiredCPUCycle());

    std::vector<int> hopPath(task->getHopPath());
    dupTask->setHopPath(hopPath);
    dupTask->setSubTaskVec({subTaskVec[subTaskIdx]});
    return dupTask;
}

double TaskQueue::getRegionLoad(int exceptServerId) {
    std::vector<int> servers;
    for (int i = 0; i < neighborServers.size(); ++i) {
        if (neighborServers[i] == exceptServerId)
            continue;
        servers.push_back(neighborServers[i]);
    }

    double regionLoad = TaskQueue::getAvgLoad(servers);
    return regionLoad;
}

double TaskQueue::getAvgLoad(const std::vector<int>& servers) {
    Enter_Method("getAvgLoad()");

    double avgLoad;
    TaskQueue *taskQueueModule;
    ServerStatus *serverStatus;
    for (int i = 0; i < servers.size(); ++i) {
        taskQueueModule = omnetpp::check_and_cast<TaskQueue*>(getSystemModule()->getSubmodule("edgeServer", servers[i])->getSubmodule("taskQueue"));
        serverStatus = taskQueueModule->getServerStatus();
        avgLoad = serverStatus->getTotalRequiredCycle() / serverStatus->getServerFrequency();
        delete serverStatus;
        delete taskQueueModule;
    }
    avgLoad /= servers.size();

    return avgLoad;
}

void TaskQueue::refreshDisplay() const
{

}

void TaskQueue::finish()
{
}
