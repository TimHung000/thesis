//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __SOURCE_H
#define __SOURCE_H
#include <omnetpp.h>

class Task;

class Source : public omnetpp::cSimpleModule
{
    private:
        int taskCounter;
        std::string taskName;
        omnetpp::simsignal_t createdSignal;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(omnetpp::cMessage *msg) override;
        virtual Task *createTask();
        virtual void finish() override;
};

#endif


