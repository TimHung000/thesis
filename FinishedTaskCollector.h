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
    omnetpp::simsignal_t isCompletedSignal;
    omnetpp::simsignal_t splitTaskPartialCompleteSignal;
    omnetpp::simsignal_t splitTaskCompleteSignal;
    omnetpp::simsignal_t taskTimeSignal;
    omnetpp::simsignal_t rewardSignal;
    omnetpp::simsignal_t completedDelayToleranceSignal;
    omnetpp::simsignal_t delayToToleranceMultipleSignal;
    std::unordered_map<int64_t, std::pair<std::vector<Task*>, int>> taskMap;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void finish() override;
    void emitSignal(Task *task);
    void emitSignal(std::vector<Task*>&);
};

#endif
