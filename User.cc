#include "User.h"

#include "Task_m.h"
#include "SubTask.h"
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <map>

Define_Module(User);

void User::initialize()
{

    userId = getIndex();

    std::string hotSpotServer = par("hotSpotServer").stdstringValue();
    std::stringstream ss(hotSpotServer);
    std::vector<int> hotSpot;
    int tmp;
    while (ss >> tmp)
        hotSpot.push_back(tmp);

    double hotSpotLoad = par("hotSpotLoad").doubleValue();
    int serverCount = getParentModule()->getSubmoduleVectorSize("edgeServer");
    int threshold = std::floor(getParentModule()->par("numUsers").intValue() * (1 - hotSpotLoad));

    if (userId < threshold)
        serverId = intuniform(0, serverCount-1);
    else {
        int idx = intuniform(0, hotSpot.size()-1);
        serverId = hotSpot[idx];
    }

    serverTaskInGate = getParentModule()->getSubmodule("edgeServer", serverId)->gate("userTaskIn");


    taskCounter = 0;
    minRequiredCPUCycle = par("minRequiredCPUCycle").doubleValue();                   // M
    maxRequiredCPUCycle = par("maxRequiredCPUCycle").doubleValue();                   // M
    taskSizeMultiple = par("taskSizeMultiple").doubleValue();
    minDelayTolerance = par("minDelayTolerance").doubleValue();                       // ms
    maxDelayTolerance = par("maxDelayTolerance").doubleValue();                       // ms
    minSubTaskCount = static_cast<int>(par("minSubTaskCount").doubleValue());
    maxSubTaskCount = static_cast<int>(par("maxSubTaskCount").doubleValue());

    // schedule the first message timer for start time
    scheduleAt(omnetpp::simTime() + par("interArrivalTime").doubleValue(), new omnetpp::cMessage("newTaskTimer"));
}

void User::handleMessage(omnetpp::cMessage *msg)
{
    ASSERT(msg->isSelfMessage());

    if (msg->isSelfMessage()) {
        scheduleAt(omnetpp::simTime() + par("interArrivalTime").doubleValue(), msg);
        Task *task = createTask();
        sendDirect(task, serverTaskInGate);
    } else {
        delete msg;
    }
}

Task *User::createTask()
{
    char buf[80];
    sprintf(buf, "task%d-server%d-user%d", ++taskCounter, 0, userId);
    Task *task = new Task(buf);

    task->setTaskId(task->getId());
    task->setCreationTime(omnetpp::simTime());
    task->setTotalWaitingTime(omnetpp::SimTime::ZERO);
    task->setTotalProcessingTime(omnetpp::SimTime::ZERO);
    task->setTotalPropagationTime(omnetpp::SimTime::ZERO);
    task->setFinishedTime(omnetpp::SimTime::ZERO);

    int randRequiredCPUCycle = minRequiredCPUCycle + intrand(maxRequiredCPUCycle - minRequiredCPUCycle + 1);
    double requiredCPUCycle = static_cast<double>(randRequiredCPUCycle) * 1e6;
    task->setRequiredCycle(requiredCPUCycle);

    double taskSize = requiredCPUCycle * taskSizeMultiple;
    task->setTaskSize(taskSize);

    int randDelayTolerance = minDelayTolerance + intrand(maxDelayTolerance - minDelayTolerance + 1);
    double delayTolerance = static_cast<double>(randDelayTolerance) * 1e-3;
    task->setDelayTolerance(delayTolerance);

    task->setProcessedCycle(0.0);
    task->setArrivingServer(serverId);
    task->setRunningServer(-1);
    task->setDestinationServer(serverId);
    task->setHopCount(0);
    task->setIsCompleted(false);

    int randSubTaskCount = minSubTaskCount + intrand(maxSubTaskCount - minSubTaskCount + 1);
    task->setTotalSubTaskCount(randSubTaskCount);

    std::vector<int> weightVec;
    int totalWeight = 0;
    int curRand;
    for (int i = 0; i < randSubTaskCount; ++i) {
        curRand = 1 + intrand(5);  // 1~5
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

    double totalSubTaskSize = 0;
    double totalSubTaskRequiredCPUCycle = 0;

    for (int i = 0; i < subTaskVec.size(); ++i){
        totalSubTaskSize += task->getSubTaskVec()[i]->getSubTaskSize();
        totalSubTaskRequiredCPUCycle += task->getSubTaskVec()[i]->getSubTaskRequiredCPUCycle();
    }
    ASSERT(task->getTaskSize() > 0);
    ASSERT(totalSubTaskRequiredCPUCycle > 0);
    ASSERT(totalSubTaskSize == task->getTaskSize());
    ASSERT(totalSubTaskRequiredCPUCycle == task->getRequiredCycle());

    return task;
}

void User::finish()
{
}