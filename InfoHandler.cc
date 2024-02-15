#include <omnetpp.h>
#include "InfoHandler.h"

Define_Module(InfoHandler);

void InfoHandler::initialize()
{
    for (auto& it: edgeServerStatus) {
        cancelAndDelete(it.second);
    }
    edgeServerStatus.clear();
    maxHop = par("maxHop");
    serverId = getParentModule()->getIndex();
}

void InfoHandler::handleMessage(omnetpp::cMessage *msg)
{
    // get current edge server status info
    if (msg->getArrivalGate() == gate("processorInfoIn")) {
        // send info message to dispatcher
        omnetpp::cMessage *dupMsg = msg->dup();
        omnetpp::cGate *curGate;
        int n = gateSize("otherEdgeServerInfoPorts$o");
        for (int i = 0; i < n; ++i) {
            dupMsg = msg->dup();
            curGate = gate("otherEdgeServerInfoPorts$o", i);
            send(dupMsg, curGate);
        }
        cancelAndDelete(msg);
    } else { // get other edge server status info
        Info *incomingInfo = omnetpp::check_and_cast<Info*>(msg);
        incomingInfo->setHopCount(incomingInfo->getHopCount()+1);
        EV << "incomingInfo is from server " << incomingInfo->getServerIdx() << ". It's creation time is "
                << incomingInfo->getCreationTime() <<  ", hopCount is "
                << incomingInfo->getHopCount() << omnetpp::endl;

        showStoreStatus();

        if (incomingInfo->getServerIdx() == serverId) {
            EV << "get the info from it's own server, so drop it" << omnetpp::endl;
            cancelAndDelete(incomingInfo);
            return;
        }

        Info *originInfo = nullptr;
        if (edgeServerStatus.find(incomingInfo->getServerIdx()) != edgeServerStatus.end())
            originInfo = edgeServerStatus[incomingInfo->getServerIdx()];

        // already update the current time stamp info
        if (originInfo && incomingInfo->getCreationTime()  == originInfo->getCreationTime()
                && incomingInfo->getHopCount() >= originInfo->getHopCount()) {
            EV << "this info already exist so do not update it" << omnetpp::endl;
            cancelAndDelete(incomingInfo);
            return;
        }

        // update edge server status to incoming info
        if (originInfo)
            cancelAndDelete(originInfo);
        EV << "update server " << incomingInfo->getServerIdx() << " status" << omnetpp::endl;
        edgeServerStatus[incomingInfo->getServerIdx()] = incomingInfo;

        showStoreStatus();

        // broadcast to neighbor
        if (incomingInfo->getHopCount() == maxHop) {
            EV << "this info is already at max hop, so do not broadcast it" << omnetpp::endl;
            return;
        }
        Info *dupInfo;
        omnetpp::cGate *curGate;
        int n = gateSize("otherEdgeServerInfoPorts$o");
        for (int i = 0; i < n; ++i) {
            dupInfo = incomingInfo->dup();
            curGate = gate("otherEdgeServerInfoPorts$o", i);
            send(dupInfo, curGate);
        }
        EV << "finished broadcast to neighbor" << omnetpp::endl;
    }
}

void InfoHandler::finish()
{
    for (auto& it: edgeServerStatus) {
        cancelAndDelete(it.second);
    }
    edgeServerStatus.clear();
}

void InfoHandler::showStoreStatus()
{
    if (serverId == 0) {
        EV << omnetpp::endl;
        EV << "server   creation   hop" << omnetpp::endl;
        for (const auto& entry: edgeServerStatus) {
            Info* info = entry.second;
            EV <<  entry.first << "     " << info->getCreationTime() << "     " << info->getHopCount() << omnetpp::endl;
        }
        EV << omnetpp::endl;
    }
}

std::unordered_map<int, Info*> InfoHandler::getEdgeServerStatus()
{
    return edgeServerStatus;
}
