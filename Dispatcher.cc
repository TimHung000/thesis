/*
 * Dispatcher.cc
 *
 *  Created on: Jan 29, 2024
 *      Author: tim
 */




//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <vector>
#include <random>
#include <string>
#include "Dispatcher.h"
#include "Processor.h"
#include "Task_m.h"
#include "Info_m.h"

Define_Module(Dispatcher);

Dispatcher::~Dispatcher()
{

}

void Dispatcher::initialize()
{

}

void Dispatcher::handleMessage(omnetpp::cMessage *msg)
{
    if (msg->getArrivalGate() == this->gate("infoIn")) {
        EV << "get info from info collector" << omnetpp::endl;
        Info *info = omnetpp::check_and_cast<Info*>(msg);
        std::string serverName = info->getServerName();
        edgeServerStatus[serverName] = info;
        EV << edgeServerStatus[serverName]->getServerName() << omnetpp::endl;
    } else {
        EV << "receiving a task to dispatch" << omnetpp::endl;
        Task *incomingTask = omnetpp::check_and_cast<Task*>(msg);
        send(incomingTask, "myServerOut");
    }
}

void Dispatcher::refreshDisplay() const
{
    // Convert taskName to std::string
    std::string statusTag = "execute";

    // Set the tag argument in the display string
    getDisplayString().setTagArg("processor", 0, statusTag.c_str());}

void Dispatcher::finish()
{
}
