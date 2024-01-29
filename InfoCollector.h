#ifndef __InfoCollector_H
#define __InfoCollector_H

#include <omnetpp.h>

class InfoCollector : public omnetpp::cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void finish() override;
};

#endif

