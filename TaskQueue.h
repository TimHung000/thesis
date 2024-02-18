#ifndef __TaskQueue_H
#define __TaskQueue_H

#include <omnetpp.h>
#include <unordered_map>
#include <string>
#include <vector>
#include "ServerStatus.h"
#include "Task_m.h"

class TaskQueue : public omnetpp::cSimpleModule
{
    private:
        // server basic info
        int serverId;
        std::vector<int> neighborSevers;

        // initialize server capacity
        double serverFrequency;
        double serverMemory;

        // initialize server status
        double totalRequiredCycle;
        double totalMemoryConsumed;


        std::string dispatchingAlgo;
        std::string schedulingAlgo;

        std::list<Task*> garbageQueue;

        std::list<Task*> FIFOwaitingQueue;

        std::list<Task*> wholeTaskWaitingQueue;       // for non split task
        std::list<Task*> subTaskWaitingQueue;        // for sub task

        // for random algo used
        int maximumHop;
        double memoryThreshold;

        // for processing task management
        Task *runningTask;


        // signal
        omnetpp::simsignal_t memoryLoadingSignal;
        omnetpp::simsignal_t CPULoadingSignal;
        omnetpp::simsignal_t totalTaskFinishedSignal;
        omnetpp::simsignal_t totalTaskCompletedSignal;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(omnetpp::cMessage *msg) override;
        virtual void refreshDisplay() const override;
        virtual void finish() override;
        void randomDispatchingAlgo(omnetpp::cMessage *msg);
        void greedyDispatchingAlgo(omnetpp::cMessage *msg);
        void proposedDispatchingAlgo(omnetpp::cMessage *msg);
        void FIFOSchedulingAlgo();
        void proposedSchedulingAlgo();
        void scheduling();
    public:
        ~TaskQueue();
        ServerStatus *getServerStatus();


};

#endif


