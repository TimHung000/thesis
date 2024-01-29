//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __MERGE_H
#define __MERGE_H

#include <omnetpp.h>

/**
 * All messages received on any input gate will be sent out on the output gate
 */
class Merge : public omnetpp::cSimpleModule
{
    protected:
        virtual void handleMessage(omnetpp::cMessage *msg) override;
};

#endif

