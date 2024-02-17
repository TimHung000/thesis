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
    maxFrequency = par("maxFrequency").doubleValue();
    minFrequency = par("minFrequency").doubleValue();
    memoryMultiple = par("memoryMultiple").doubleValue();
    schedulingAlgo = par("schedulingAlgo").stdstringValue();
    EV << "schedulingAlog:" << schedulingAlgo << omnetpp::endl;
    double randFrequency = minFrequency + intrand(maxFrequency - minFrequency + 1);
    frequency = static_cast<double>(randFrequency) * 1e9;

    serverCapacity = frequency * memoryMultiple;

    totalRequiredCycle = 0;
    totalMemoryConsumed = 0;
    endServiceMsg = new omnetpp::cMessage("end-service");
    statusReportMsg = new omnetpp::cMessage("statusReport");
    taskRunning = nullptr;
    if (getParentModule()->getSubmodule("infoHandler")->par("maxHop").intValue() > 0) {
        scheduleAt(0, statusReportMsg);
        EV << "processor need to shared status to info handler" << omnetpp::endl;
    }
    statusUpadteInterval = par("statusUpadteInterval").doubleValue() * 1e-3;


    totalTaskFinished = 0;
    totalTaskCompleted = 0;
    memoryLoadingSignal = registerSignal("memoryLoading");
    CPULoadingSignal = registerSignal("CPULoading");
    totalTaskFinishedSignal = registerSignal("totalTaskFinished");
    totalTaskCompletedSignal = registerSignal("totalTaskCompleted");
    reportMsg = new omnetpp::cMessage("reportMsg");
    scheduleAt(0, reportMsg);
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
        EV << "finished processed one task and release memory, before memory release: " << totalMemoryConsumed << omnetpp::endl;
        totalRequiredCycle -= taskRunning->getRequiredCycle();
        totalMemoryConsumed -= taskRunning->getTaskSize();
        EV << "finished processed one task and release memory, after memory release: " << totalMemoryConsumed << omnetpp::endl;
        totalTaskFinished += 1;
        totalTaskCompleted += 1;
        send(taskRunning, "taskFinishedOut");
        taskRunning = nullptr;

        scheduling();

        return;
    }

    // update the server info to dispatcher
    if (msg == statusReportMsg) {
        Info *InfoMsg = createEdgeServerInfoMsg();
        send(InfoMsg, "infoOut");
        scheduleAfter(statusUpadteInterval, statusReportMsg);
        return;
    }

    if (msg == reportMsg) {

        emit(memoryLoadingSignal, totalMemoryConsumed / serverCapacity);
        emit(CPULoadingSignal, totalRequiredCycle / frequency);
        emit(totalTaskFinishedSignal, totalTaskFinished);
        emit(totalTaskCompletedSignal, totalTaskCompleted);
        totalTaskFinished = 0;
        totalTaskCompleted = 0;
        scheduleAfter(1, reportMsg);
        return;
    }


    // new task incoming from user or other server offloading their task
    Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);
    incomingTask->setRunningServer(serverId);
    totalRequiredCycle += incomingTask->getRequiredCycle();
    totalMemoryConsumed += incomingTask->getTaskSize();

    if (schedulingAlgo == "FIFO") {
        FIFOwaitingQueue.push_back(incomingTask);

    } else if (schedulingAlgo == "Proposed") {

        bool isSubTask = incomingTask->getSubTaskVec().size() != incomingTask->getTotalSubTaskCount();
        if (!isSubTask) {
            omnetpp::simtime_t incomingTaskDeadline = incomingTask->getCreationTime() + incomingTask->getDeadline();
            omnetpp::simtime_t curTaskDeadline;
            std::list<Task*>::iterator it = wholeTaskWaitingQueue.begin();
            while (it != wholeTaskWaitingQueue.end()) {
                curTaskDeadline = (*it)->getCreationTime() + (*it)->getDeadline();
                if (curTaskDeadline > incomingTaskDeadline)
                    break;
                ++it;
            }
            wholeTaskWaitingQueue.insert(it, incomingTask);
        } else {
            std::list<Task*>::iterator it = subTaskWaitingQueue.begin();
            omnetpp::simtime_t incomingTaskDeadline = incomingTask->getCreationTime() + incomingTask->getDeadline();
            omnetpp::simtime_t curTaskDeadline;
            while (it != subTaskWaitingQueue.end()) {
                curTaskDeadline = (*it)->getCreationTime() + (*it)->getDeadline();
                if (curTaskDeadline > incomingTaskDeadline)
                    break;
                ++it;
            }
            subTaskWaitingQueue.insert(it, incomingTask);
        }
    }

    scheduling();
}

void Processor::refreshDisplay() const
{
}

void Processor::finish()
{
}

void Processor::scheduling()
{
    if (schedulingAlgo == "FIFO") {
        std::list<Task*>::iterator it = FIFOwaitingQueue.begin();
        while (it != FIFOwaitingQueue.end()) {
            if (omnetpp::simTime() > (*it)->getCreationTime() + (*it)->getDeadline()) {
                totalTaskFinished += 1;
                send((*it), "taskFinishedOut");
                it = FIFOwaitingQueue.erase(it);
            } else
                ++it;
        }

        // still has task running
        if (taskRunning)
            return;

        // get the first task that can finished before deadline
        it = FIFOwaitingQueue.begin();
        while (it != FIFOwaitingQueue.end() &&
                omnetpp::simTime() + (*it)->getRequiredCycle() / frequency > (*it)->getCreationTime() + (*it)->getDeadline()) {

            totalTaskFinished += 1;
            totalRequiredCycle -= (*it)->getRequiredCycle();
            totalMemoryConsumed -= (*it)->getTaskSize();
            send((*it), "taskFinishedOut");

            ++it;
            FIFOwaitingQueue.pop_front();
        }

        // assing the first task than can finished before deadline to the taskRunning
        if (it != FIFOwaitingQueue.end()) {
            FIFOwaitingQueue.pop_front();
            taskRunning = *it;
            double runningTime = taskRunning->getRequiredCycle() / frequency;
            scheduleAt(omnetpp::simTime()+runningTime, endServiceMsg);
        }

    } else if (schedulingAlgo == "Proposed") {
        if (taskRunning)
            return;

        // remove all task that will exceed the deadline
        std::list<Task*>::iterator it = wholeTaskWaitingQueue.begin();
        while (it != wholeTaskWaitingQueue.end() &&
                omnetpp::simTime() + (*it)->getRequiredCycle() / frequency > (*it)->getCreationTime() + (*it)->getDeadline()) {

            totalTaskFinished += 1;
            totalRequiredCycle -= (*it)->getRequiredCycle();
            totalMemoryConsumed -= (*it)->getTaskSize();
            send((*it), "taskFinishedOut");
            ++it;
            wholeTaskWaitingQueue.pop_front();
        }

        it = subTaskWaitingQueue.begin();
        while (it != subTaskWaitingQueue.end() &&
                omnetpp::simTime() + (*it)->getRequiredCycle() / frequency > (*it)->getCreationTime() + (*it)->getDeadline()) {

            totalTaskFinished += 1;
            totalRequiredCycle -= (*it)->getRequiredCycle();
            totalMemoryConsumed -= (*it)->getTaskSize();
            send((*it), "taskFinishedOut");
            ++it;
            subTaskWaitingQueue.pop_front();
        }

//        std::list<Task*>::iterator wholeTaskIt = wholeTaskWaitingQueue.begin();
//        std::list<Task*>::iterator subTaskIt = subTaskWaitingQueue.begin();
//        if (wholeTaskIt != wholeTaskWaitingQueue.end() && subTaskIt != subTaskWaitingQueue.end()) {
//            // if subtask can meet the deadline after running the wholeTask first then run the whole task
//            if (omnetpp::simTime() + ((*wholeTaskIt)->getRequiredCycle() + (*subTaskIt)->getRequiredCycle()) / frequency <=
//                (*subTaskIt)->getCreationTime() + (*subTaskIt)->getDeadline()) {
//                taskRunning = *wholeTaskIt;
//                wholeTaskWaitingQueue.pop_front();
//
//            } else {
//                taskRunning = *subTaskIt;
//                subTaskWaitingQueue.pop_front();
//            }
//        } else if (wholeTaskIt != wholeTaskWaitingQueue.end() && subTaskIt == subTaskWaitingQueue.end()) {
//            taskRunning = *wholeTaskIt;
//            wholeTaskWaitingQueue.pop_front();
//        } else if (subTaskIt != subTaskWaitingQueue.end() && wholeTaskIt == wholeTaskWaitingQueue.end()) {
//            taskRunning = *subTaskIt;
//            subTaskWaitingQueue.pop_front();
//        }

        std::list<Task*>::iterator subTaskIt = subTaskWaitingQueue.begin();
        std::list<Task*>::iterator wholeTaskIt = wholeTaskWaitingQueue.begin();
        if (subTaskIt != subTaskWaitingQueue.end()) {
            taskRunning = *subTaskIt;
            subTaskWaitingQueue.pop_front();
        } else if (wholeTaskIt != wholeTaskWaitingQueue.end()) {
            taskRunning = *wholeTaskIt;
            wholeTaskWaitingQueue.pop_front();
        }

        if (taskRunning) {
            double runningTime = taskRunning->getRequiredCycle() / frequency;
            scheduleAt(omnetpp::simTime()+runningTime, endServiceMsg);
        }

    }


}

Info *Processor::createEdgeServerInfoMsg()
{
    Info *serverInfo = new Info();
    serverInfo->setServerIdx(getParentModule()->getIndex());
    serverInfo->setServerName(getParentModule()->getName());
    serverInfo->setServerFrequency(frequency);
    serverInfo->setServerCapacity(serverCapacity);
    if (schedulingAlgo == "FIFO") {
        serverInfo->setTaskCount(taskRunning ? FIFOwaitingQueue.size() + 1 : FIFOwaitingQueue.size());

    } else if (schedulingAlgo == "Proposed") {
        serverInfo->setTaskCount(taskRunning ? wholeTaskWaitingQueue.size() + subTaskWaitingQueue.size() + 1
                : wholeTaskWaitingQueue.size() + subTaskWaitingQueue.size());
    }
    serverInfo->setTotalRequiredCycle(totalRequiredCycle);
    serverInfo->setTotalMemoryConsumed(totalMemoryConsumed);
    serverInfo->setHopCount(0);
    serverInfo->setCreationTime(omnetpp::simTime());
    return serverInfo;
}
