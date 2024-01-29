#ifndef __Processor_H
#define __Processor_H

#include <omnetpp.h>
#include "Info_m.h"


class Task;

class Processor : public omnetpp::cSimpleModule
{
    private:
        std::list<Task*> waitingQueue;    // unfinshed or unprocessed tasks
        Task *taskRunning = nullptr;                     // the task that is processing
        double frequency;
        double totalRequiredCycles;


        omnetpp::cMessage *endServiceMsg = nullptr;
        omnetpp::cMessage *statusReportMsg = nullptr;
        omnetpp::simsignal_t busySignal;
        omnetpp::simsignal_t cpuFrequencySignal;


    protected:
        virtual void initialize() override;
        virtual void handleMessage(omnetpp::cMessage *msg) override;
        virtual void refreshDisplay() const override;
        virtual void finish() override;
        void runNextTask();
        Info *createEdgeServerInfoMsg();

    public:
        ~Processor();


};

#endif


