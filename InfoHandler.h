#ifndef __InfoHandler_H
#define __InfoHandler_H

#include <omnetpp.h>
#include <unordered_map>
#include "Info_m.h"

class InfoHandler : public omnetpp::cSimpleModule
{
private:
    std::unordered_map<int, Info*> edgeServerStatus;

protected:
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void finish() override;
public:
    std::unordered_map<int, Info*> getEdgeServerStatus();
};

#endif

