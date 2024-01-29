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
    busySignal = registerSignal("busy");
    cpuFrequencySignal = registerSignal("cpuFrequency");
    emit(busySignal, false);
    WATCH_PTR(taskRunning);
    std::vector<double> frequencyList = {1000000000, 2000000000, 3000000000};
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(0, frequencyList.size() - 1);
    frequency = frequencyList[distr(eng)];
    emit(cpuFrequencySignal, frequency);
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
        omnetpp::simtime_t d = omnetpp::simTime() - endServiceMsg->getSendingTime();
        taskRunning->setTotalProcessingTime(taskRunning->getTotalProcessingTime() + d);
        EV << "task processing time" << d << omnetpp::endl;
        taskRunning->setProcessedCycles(taskRunning->getProcessedCycles() + d.dbl() * frequency);
        EV << "task processed cycles" << d.dbl() * frequency << omnetpp::endl;
        taskRunning->setRunningServer(getParentModule()->par("address").intValue());
        send(taskRunning, "taskCompleteOut");
        taskRunning = nullptr;
        emit(busySignal, false);

        if (!waitingQueue.empty() && !taskRunning)
            runNextTask();

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
    waitingQueue.push_back(incomingTask);
    if (!waitingQueue.empty() && !taskRunning)
        runNextTask();
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

void Processor::runNextTask()
{
    taskRunning = waitingQueue.front();
    waitingQueue.pop_front();
    double cpuCycles = taskRunning->getCpuCycles();
    double runningTime = cpuCycles / frequency;
    EV << "task " << taskRunning->getName() << " is running and required" << cpuCycles << " cycles" << omnetpp::endl;
    EV << "sever cpu frequency is " << frequency << omnetpp::endl;
    EV << "running time is " << runningTime << omnetpp::endl;

    scheduleAt(omnetpp::simTime()+runningTime, endServiceMsg);
    emit(busySignal, true);
}

Info *Processor::createEdgeServerInfoMsg()
{
    Info *serverInfo = new Info();
    serverInfo->setServerId(getParentModule()->getId());
    serverInfo->setServerName(getParentModule()->getName());
    serverInfo->setServerFrequency(frequency);
    serverInfo->setTaskCount(taskRunning ? waitingQueue.size() + 1 : waitingQueue.size());
    serverInfo->setTotalRequiredCycle(totalRequiredCycles);
    return serverInfo;
}
