#ifndef __FinishedTaskCollector_H
#define __FinishedTaskCollector_H

#include <omnetpp.h>
#include <unordered_map>
#include <cstdint>
#include "Task_m.h"
#include <vector>
#include <utility>

class FinishedTaskCollector : public omnetpp::cSimpleModule
{
  private:
//    omnetpp::simsignal_t taskIdSignal;
//    omnetpp::simsignal_t creationTimeSignal;
//    omnetpp::simsignal_t totalWaitingTimeSignal;
//    omnetpp::simsignal_t totalProcessingTimeSignal;
//    omnetpp::simsignal_t totalPropagationTimeSignal;
//    omnetpp::simsignal_t deadlineSignal;
//    omnetpp::simsignal_t finishedTimeSignal;
//    omnetpp::simsignal_t taskSizeSignal;
//    omnetpp::simsignal_t requiredCycleSignal;
//    omnetpp::simsignal_t processedCyclesSignal;
//    omnetpp::simsignal_t arrivingServerSignal;
//    omnetpp::simsignal_t runningServerSignal;
//    omnetpp::simsignal_t hopCountSignal;
    omnetpp::simsignal_t isCompletedSignal;
//    omnetpp::simsignal_t totalSubTaskCountSignal;
    omnetpp::simsignal_t splitTaskPartialCompleteSignal;
    omnetpp::simsignal_t splitTaskCompleteSignal;
    omnetpp::simsignal_t totalTimeSignal;
    std::unordered_map<int64_t, std::pair<std::vector<Task*>, int>> taskMap;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void finish() override;
    void emitSignal(Task *task);
    void emitSignal(std::vector<Task*>&);
};

#endif
