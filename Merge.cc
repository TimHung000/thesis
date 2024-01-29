//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Merge.h"

Define_Module(Merge);

void Merge::handleMessage(omnetpp::cMessage *msg)
{
    send(msg, "out");
}

