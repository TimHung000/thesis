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
    }
    else {
        // finished
        delete msg;
    }
}

Task *Source::createTask()
{
    char buf[80];
    sprintf(buf, "%.60s-%d", taskName.c_str(), ++taskCounter);
    Task *task = new Task(buf);
    int arrivingServer = getParentModule()->par("address");
    task->setArrivingServer(arrivingServer);
    return task;
}

void Source::finish()
{
    emit(createdSignal, taskCounter);
}
