//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __USER_H
#define __USER_H
#include <omnetpp.h>
#include <vector>

class Task;

class User : public omnetpp::cSimpleModule
{
    private:
        int serverId;
        int userId;
        int taskCounter;
        std::vector<double> taskSizeVector;
        std::vector<double> taskDeadlineVector;
        std::vector<double> taskCpuCyclesVector;
        omnetpp::cGate *serverTaskInGate;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(omnetpp::cMessage *msg) override;
        virtual Task *createTask();
        virtual void finish() override;
};

#endif


