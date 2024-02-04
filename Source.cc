#include "Source.h"
#include "Task_m.h"
#include <string>


Define_Module(Source);

void Source::initialize()
{

    createdSignal = registerSignal("created");
    taskCounter = 0;
    WATCH(taskCounter);
    taskName = par("taskName").stringValue();
    if (taskName == "")
        taskName = getName();

    taskSizeVector = {1 * 1e6, 2 * 1e6, 3 * 1e6};
    taskCpuCyclesVector = {5 * 1e6, 10 * 1e6, 15 * 1e6};
    taskDeadlineVector = {40, 50, 60};

    // schedule the first message timer for start time
    scheduleAt(omnetpp::simTime() + par("interArrivalTime").doubleValue(), new omnetpp::cMessage("newTaskTimer"));
}

void Source::handleMessage(omnetpp::cMessage *msg)
{
    ASSERT(msg->isSelfMessage());

    if (msg->isSelfMessage()) {
        // reschedule the timer for the next message
        scheduleAt(omnetpp::simTime() + par("interArrivalTime").doubleValue(), msg);

        Task *task = createTask();
        send(task, "out");
    } else {
        // finished
        delete msg;
    }
}

Task *Source::createTask()
{
    char buf[80];
    sprintf(buf, "%.60s-%d", taskName.c_str(), ++taskCounter);
    Task *task = new Task(buf);
    task->setCreationTime(omnetpp::simTime());
    task->setTotalWaitingTime(omnetpp::SimTime::ZERO);
    task->setTotalProcessingTime(omnetpp::SimTime::ZERO);
    task->setTotalPropagationTime(omnetpp::SimTime::ZERO);
    int randInt = intrand(taskSizeVector.size() - 1);
    task->setTaskSize(taskSizeVector[randInt]);
    task->setCpuCycles(taskCpuCyclesVector[randInt]);
    task->setDeadline(taskDeadlineVector[randInt]);
    task->setProcessedCycles(0.0);
    task->setArrivingServer(getParentModule()->getIndex());
    task->setRunningServer(-1);
    task->setHopCount(0);
    task->setIsCompleted(false);
    return task;
}

void Source::finish()
{
    emit(createdSignal, taskCounter);
}
