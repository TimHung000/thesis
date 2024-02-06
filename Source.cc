#include "Source.h"
#include "Task_m.h"
#include "SubTask.h"
#include <string>
#include <vector>
#include <cmath>

Define_Module(Source);

void Source::initialize()
{
    serverId = getParentModule()->getIndex();
    userId = getIndex();
    taskCounter = 0;
    minTaskSize = 1;    // MB
    maxTaskSize = 3;    // MB
    minDeadline = 50;   // ms
    maxDeadline = 100;  // ms
    minSubTaskCount = 1;
    maxSubTaskCount = 5;

    // schedule the first message timer for start time
    scheduleAt(omnetpp::simTime() + par("interArrivalTime").doubleValue(), new omnetpp::cMessage("newTaskTimer"));
}

void Source::handleMessage(omnetpp::cMessage *msg)
{
    ASSERT(msg->isSelfMessage());

    if (msg->isSelfMessage()) {
        scheduleAt(omnetpp::simTime() + par("interArrivalTime").doubleValue(), msg);
        Task *task = createTask();
        send(task, "out");
    } else {
        delete msg;
    }
}

Task *Source::createTask()
{
    char buf[80];
    sprintf(buf, "task%d-server%d-user%d", ++taskCounter, serverId, userId);
    Task *task = new Task(buf);

    task->setTaskId(task->getId());
    task->setCreationTime(omnetpp::simTime());
    task->setTotalWaitingTime(omnetpp::SimTime::ZERO);
    task->setTotalProcessingTime(omnetpp::SimTime::ZERO);
    task->setTotalPropagationTime(omnetpp::SimTime::ZERO);
    int randTaskSize = minTaskSize + intrand(maxTaskSize-minTaskSize);
    double taskSize = static_cast<double>(randTaskSize) * 1e6;
    task->setTaskSize(taskSize);
    double requiredCPUCycle = taskSize * 5;
    task->setRequiredCycle(requiredCPUCycle);
    int randDeadline = minDeadline + intrand(maxDeadline-minDeadline);
    double deadline = static_cast<double>(randDeadline) * 1e-3;
    task->setDeadline(deadline);
    task->setProcessedCycle(0.0);
    task->setArrivingServer(serverId);
    task->setRunningServer(-1);
    task->setDestinationServer(serverId);
    task->setHopCount(0);
    task->setIsCompleted(false);
    int randSubTaskCount = minSubTaskCount + intrand(maxSubTaskCount-minSubTaskCount);
    task->setTotalSubTaskCount(randSubTaskCount);

    std::vector<int> weightVec;
    int totalWeight;
    int curRand;
    for (int i = 0; i < randSubTaskCount; ++i) {
        curRand = 1 + intrand(5);
        weightVec.push_back(curRand);
        totalWeight += curRand;
    }

    subTaskVector& subTaskVec = task->getSubTaskVecForUpdate();
    double remainedTaskSize = taskSize;
    double remainedRequiredCPUCycle = requiredCPUCycle;
    double curSubTaskSize;
    double curSubTaskRequiredCPUCycle;
    SubTask *subTask;
    for (int i = 0; i < randSubTaskCount-1; ++i) {
        curSubTaskSize = std::floor(taskSize / totalWeight * weightVec[i]);
        curSubTaskRequiredCPUCycle = std::floor(requiredCPUCycle / totalWeight * weightVec[i]);
        subTask = new SubTask(i, curSubTaskSize, curSubTaskRequiredCPUCycle);
        subTaskVec.push_back(subTask);
        remainedTaskSize -= curSubTaskSize;
        remainedRequiredCPUCycle -= curSubTaskRequiredCPUCycle;
    }
    subTask = new SubTask(randSubTaskCount-1, remainedTaskSize, remainedRequiredCPUCycle);
    subTaskVec.push_back(subTask);

    return task;
}

void Source::finish()
{
}
