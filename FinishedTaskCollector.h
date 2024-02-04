#ifndef __FinishedTaskCollector_H
#define __FinishedTaskCollector_H

#include <omnetpp.h>

class FinishedTaskCollector : public omnetpp::cSimpleModule
{
  private:
    omnetpp::simsignal_t lifeTimeSignal;
    omnetpp::simsignal_t totalWaitingTimeSignal;
    omnetpp::simsignal_t totalProcessingTimeSignal;
    omnetpp::simsignal_t totalPropagationTimeSignal;
    omnetpp::simsignal_t deadlineSignal;
    omnetpp::simsignal_t taskSizeSignal;
    omnetpp::simsignal_t cpuCyclesSignal;
    omnetpp::simsignal_t processedCyclesSignal;
    omnetpp::simsignal_t arrivingServerSignal;
    omnetpp::simsignal_t runningServerSignal;
    omnetpp::simsignal_t hopCountSignal;
    omnetpp::simsignal_t isCompletedSignal;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void finish() override;
};

#endif
