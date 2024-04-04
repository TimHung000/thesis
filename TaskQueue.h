#ifndef __TaskQueue_H
#define __TaskQueue_H

#include <omnetpp.h>
#include <unordered_map>
#include <string>
#include <vector>

#include "ServerStatus.h"
#include "Task_m.h"
#include "algorithm/DispatchingAlgo.h"
#include "algorithm/SchedulingAlgo.h"


class TaskQueue : public omnetpp::cSimpleModule
{
    public:
        // server basic info
        int serverId;
        std::vector<int> neighborServers;

        // initialize server capacity
        double serverFrequency;
        double serverMemory;

        // server status
        double totalRequiredCycle;
        double totalMemoryConsumed;
        std::list<Task*> garbageQueue;
        std::list<Task*> waitingQueue;
        Task *runningTask;

        SchedulingAlgo *schedulingAlgo;
        DispatchingAlgo *dispatchingAlgo;

        // signal
        omnetpp::cMessage *serverInfoSignalMsg;
        double totalTaskFailed;
        double totalTaskCompleted;
        omnetpp::simsignal_t memoryLoadingSignal;
        omnetpp::simsignal_t CPULoadingSignal;
        omnetpp::simsignal_t totalTaskSignal;
        omnetpp::simsignal_t totalTaskCompletedSignal;
        omnetpp::simsignal_t totalTaskFailedSignal;

        ~TaskQueue();
        ServerStatus *getServerStatus();
        ServerStatus *getServerStatus(int serverId);
        double getCurrentRunningTaskFinishedCycle();

    protected:
        virtual void initialize() override;
        virtual void handleMessage(omnetpp::cMessage *msg) override;
        virtual void refreshDisplay() const override;
        virtual void finish() override;

        DispatchingAlgo *getDispatchingAlgo(std::string name, SchedulingAlgo *schedulingAlgo, TaskQueue *taskQueue);
        SchedulingAlgo *getSchedulingAlgo(std::string name, TaskQueue *taskQueue);
};

#endif


