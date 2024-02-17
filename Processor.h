#ifndef __Processor_H
#define __Processor_H

#include <omnetpp.h>
#include "Info_m.h"
#include <string>


class Task;

class Processor : public omnetpp::cSimpleModule
{
    private:
        std::list<Task*> FIFOwaitingQueue;           // unfinshed or unprocessed tasks

        std::list<Task*> wholeTaskWaitingQueue;       // for non split task
        std::list<Task*> subTaskWaitingQueue;        // for sub task

        Task *taskRunning = nullptr;      // the task that is processing
        int serverId;
        double maxFrequency;
        double minFrequency;
        double memoryMultiple;
        std::string schedulingAlgo;

        double frequency;
        double serverCapacity;
        double totalRequiredCycle;
        double totalMemoryConsumed;
        double statusUpadteInterval;


        omnetpp::cMessage *endServiceMsg = nullptr;
        omnetpp::cMessage *statusReportMsg = nullptr;
        omnetpp::cMessage *reportMsg = nullptr;

        double totalTaskFinished;
        double totalTaskCompleted;
        omnetpp::simsignal_t memoryLoadingSignal;
        omnetpp::simsignal_t CPULoadingSignal;
        omnetpp::simsignal_t totalTaskFinishedSignal;
        omnetpp::simsignal_t totalTaskCompletedSignal;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(omnetpp::cMessage *msg) override;
        virtual void refreshDisplay() const override;
        virtual void finish() override;
        void scheduling();


    public:
        ~Processor();
        Info *createEdgeServerInfoMsg();

};

#endif


