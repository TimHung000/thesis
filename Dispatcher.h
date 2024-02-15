#ifndef __Dispatcher_H
#define __Dispatcher_H

#include <omnetpp.h>
#include <unordered_map>
#include <string>
#include <vector>
#include "Info_m.h"

class Dispatcher : public omnetpp::cSimpleModule
{
    private:
        std::vector<int> neighborSevers;
        int maximumHop;
        double memoryThreshold;
        int serverId;
        std::string algorithm;
    protected:
        virtual void initialize() override;
        virtual void handleMessage(omnetpp::cMessage *msg) override;
        virtual void refreshDisplay() const override;
        virtual void finish() override;
    public:
        ~Dispatcher();


};

#endif


