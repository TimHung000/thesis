#include <vector>
#include <random>
#include "Processor.h"
#include "Task_m.h"
#include "Info_m.h"

Define_Module(Processor);

Processor::~Processor()
{
    delete taskRunning;
    cancelAndDelete(endServiceMsg);
}

void Processor::initialize()
{
    serverId = getParentModule()->getIndex();
    frequencyList = {1*1e9, 2*1e9, 3*1e9};
    serverCapacityVector = {10 * 1e6, 20 * 1e6, 30 * 1e6};
    int randInt = intrand(frequencyList.size() - 1);
    frequency = frequencyList[randInt];
    serverCapacity = serverCapacityVector[randInt];
    totalRequiredCycle = 0;
    totalMemoryConsumed = 0;
    endServiceMsg = new omnetpp::cMessage("end-service");
    statusReportMsg = new omnetpp::cMessage("statusReport");
    scheduleAt(0, statusReportMsg);
    taskRunning = nullptr;
}

void Processor::handleMessage(omnetpp::cMessage *msg)
{
    // the processor finished the current task
    if (msg == endServiceMsg) {
        ASSERT(taskRunning != nullptr);
        omnetpp::simtime_t processingTime = omnetpp::simTime() - endServiceMsg->getSendingTime();
        double processedCycle = processingTime.dbl() * frequency;
        taskRunning->setTotalProcessingTime(taskRunning->getTotalProcessingTime() + processingTime);
        taskRunning->setProcessedCycle(taskRunning->getProcessedCycle() + processedCycle);
        taskRunning->setIsCompleted(true);
        totalRequiredCycle -= taskRunning->getRequiredCycle();
        totalMemoryConsumed -= taskRunning->getTaskSize();
        send(taskRunning, "taskFinishedOut");
        taskRunning = nullptr;

        scheduling();

        return;
    }

    // update the server info to dispatcher
    if (msg == statusReportMsg) {
        Info *InfoMsg = createEdgeServerInfoMsg();
        send(InfoMsg, "infoOut");
        scheduleAfter(1, statusReportMsg);
        return;
    }


    // new task incoming from user or other server offloading their task
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);
    incomingTask->setRunningServer(serverId);
    totalRequiredCycle += incomingTask->getRequiredCycle();
    totalMemoryConsumed += incomingTask->getTaskSize();
    waitingQueue.push_back(incomingTask);
    EV << "new task in update" << omnetpp::endl;
    EV << "server total RequiredCycle: " << totalRequiredCycle << omnetpp::endl;
    EV << "server total memory consumed: " << totalMemoryConsumed << omnetpp::endl;
    scheduling();
}

void Processor::refreshDisplay() const
{
    // Convert taskName to std::string
    std::string statusTag = "execute" + (taskRunning ? std::string(" ") + taskRunning->getName() : "");

    // Set the tag argument in the display string
    getDisplayString().setTagArg("processor", 0, statusTag.c_str());}

void Processor::finish()
{
}

void Processor::scheduling()
{
    if (taskRunning || waitingQueue.empty())
        return;

    taskRunning = waitingQueue.front();
    waitingQueue.pop_front();
    double requiredCycle = taskRunning->getRequiredCycle();
    double runningTime = requiredCycle / frequency;
    EV << "scheduling new task" << omnetpp::endl;
    EV << "task required cpu cycles: " << requiredCycle << omnetpp::endl;
    EV << "server frequency: " << frequency << omnetpp::endl;
    EV << "task running time: " << runningTime << omnetpp::endl;
    while (omnetpp::simTime() + runningTime > taskRunning->getCreationTime() + taskRunning->getDeadline()) {
        EV << "task need to be drop because it can't complete before deadline: " << taskRunning->getCreationTime() + taskRunning->getDeadline() << omnetpp::endl;
        send(taskRunning, "taskFinishedOut");
        totalRequiredCycle -= taskRunning->getRequiredCycle();
        totalMemoryConsumed -= taskRunning->getTaskSize();
        EV << "server total RequiredCycle: " << totalRequiredCycle << omnetpp::endl;
        EV << "server total memory consumed: " << totalMemoryConsumed << omnetpp::endl;
        if (waitingQueue.empty()) {
            taskRunning = nullptr;
            return;
        }
        taskRunning = waitingQueue.front();
        waitingQueue.pop_front();
        requiredCycle = taskRunning->getRequiredCycle();
        runningTime = requiredCycle / frequency;
        EV << "scheduling new task" << omnetpp::endl;
        EV << "task required cpu cycles: " << requiredCycle << omnetpp::endl;
        EV << "server frequency: " << frequency << omnetpp::endl;
        EV << "task running time: " << runningTime << omnetpp::endl;
    }

    scheduleAt(omnetpp::simTime()+runningTime, endServiceMsg);
}

Info *Processor::createEdgeServerInfoMsg()
{
    Info *serverInfo = new Info();
    serverInfo->setServerIdx(getParentModule()->getIndex());
    serverInfo->setServerName(getParentModule()->getName());
    serverInfo->setServerFrequency(frequency);
    serverInfo->setServerCapacity(serverCapacity);
    serverInfo->setTaskCount(taskRunning ? waitingQueue.size() + 1 : waitingQueue.size());
    serverInfo->setTotalRequiredCycle(totalRequiredCycle);
    serverInfo->setTotalMemoryConsumed(totalMemoryConsumed);
    serverInfo->setHopCount(0);
    serverInfo->setCreationTime(omnetpp::simTime());
    return serverInfo;
}
