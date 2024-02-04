#include <omnetpp.h>
#include "InfoHandler.h"

Define_Module(InfoHandler);

void InfoHandler::initialize()
{
    for (auto& it: edgeServerStatus) {
        cancelAndDelete(it.second);
    }
    edgeServerStatus.clear();
}

void InfoHandler::handleMessage(omnetpp::cMessage *msg)
{
    omnetpp::cMessage *dupMsg;
    // get current edge server status info
    if (msg->getArrivalGate() == gate("processorInfoIn")) {
        // send info message to dispatcher
        dupMsg = msg->dup();
        int n = gateSize("otherEdgeServerInfoPorts$o");
        for (int i = 0; i < n; ++i) {
            dupMsg = msg->dup();
            omnetpp::cGate *curGate = gate("otherEdgeServerInfoPorts$o", i);
            send(dupMsg, curGate);
        }
        cancelAndDelete(msg);
    } else {
        Info *info = omnetpp::check_and_cast<Info*>(msg);
        EV << "get info from server" << info->getServerIdx() << omnetpp::endl;
        edgeServerStatus[info->getServerIdx()] = info;
    }
}

void InfoHandler::finish()
{
    for (auto& it: edgeServerStatus) {
        cancelAndDelete(it.second);
    }
    edgeServerStatus.clear();
}

std::unordered_map<int, Info*> InfoHandler::getEdgeServerStatus()
{
    return edgeServerStatus;
}
