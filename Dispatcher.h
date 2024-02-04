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
        std::unordered_map<int, Info*> edgeServerStatus;
        std::vector<int> connectedServerIndices;
        std::vector<omnetpp::cGate*> connectedGates;
        int maximumHop;
        int memoryThreshold;
    protected:
        virtual void initialize() override;
        virtual void handleMessage(omnetpp::cMessage *msg) override;
        virtual void refreshDisplay() const override;
        virtual void finish() override;
    public:
        ~Dispatcher();


};

#endif


