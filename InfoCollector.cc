#include <omnetpp.h>
#include "InfoCollector.h"

Define_Module(InfoCollector);

void InfoCollector::initialize()
{
}

void InfoCollector::handleMessage(omnetpp::cMessage *msg)
{
    omnetpp::cMessage *dupMsg;
    // get current edge server status info
    if (msg->getArrivalGate() == gate("processorInfoIn")) {
        // send info message to dispatcher
        dupMsg = msg->dup();
        send(dupMsg, "infoOut");
        int n = gateSize("otherEdgeServerInfoPorts$o");
        for (int i = 0; i < n; ++i) {
            omnetpp::cGate *curGate = gate("otherEdgeServerInfoPorts$o", i);
            dupMsg = msg->dup();
            send(dupMsg, curGate);
        }
        delete msg;
    } else {
        // get other edge server status info
        omnetpp::cGate *inputGate = msg->getArrivalGate();
        send(msg, "infoOut");
    }


}

void InfoCollector::finish()
{
    // TODO missing scalar statistics
}

