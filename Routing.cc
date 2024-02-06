#include <map>
#include <omnetpp.h>
#include "Task_m.h"
#include <cstring>

/**
 * Demonstrates static routing, utilizing the cTopology class.
 */
class Routing : public omnetpp::cSimpleModule
{
  private:
    int serverId;
    typedef std::map<int, int> RoutingTable;  // destaddr -> gateindex
    RoutingTable rtable;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
};

Define_Module(Routing);

void Routing::initialize()
{
    serverId = getParentModule()->getIndex();

    omnetpp::cTopology *topo = new omnetpp::cTopology("topo");

    std::vector<std::string> nedTypes;
    nedTypes.push_back(getParentModule()->getNedTypeName());
    EV << "nedTypes Name " << getParentModule()->getNedTypeName()<< " nodes\n";

    topo->extractByNedTypeName(nedTypes);
    EV << "cTopology found " << topo->getNumNodes() << " nodes\n";

    omnetpp::cTopology::Node *thisNode = topo->getNodeFor(getParentModule());
    std::vector<omnetpp::cTopology::Link*> linkToDelete;
    for (int i = 0; i < thisNode->getNumOutLinks(); ++i) {
        EV << "OutLink " << i << " is " << thisNode->getLinkOut(i)->getLocalGate()->getBaseName() << omnetpp::endl;
        if (std::strcmp(thisNode->getLinkOut(i)->getLocalGate()->getBaseName(), "infoPorts") == 0) {
            linkToDelete.push_back(thisNode->getLinkOut(i));
        }
    }
    for (int i = 0; i < thisNode->getNumInLinks(); ++i) {
        EV << "InLink " << i << " is " << thisNode->getLinkIn(i)->getLocalGate()->getBaseName() << omnetpp::endl;
        if (std::strcmp(thisNode->getLinkIn(i)->getLocalGate()->getBaseName(), "infoPorts") == 0) {
            linkToDelete.push_back(thisNode->getLinkIn(i));
        }
    }
    for (auto &link: linkToDelete) {
        topo->deleteLink(link);
    }
//    for (int i = 0; i < thisNode->getNumOutLinks(); ++i) {
//        EV << "OutLink " << i << " is " << thisNode->getLinkOut(i)->getLocalGate()->getBaseName() << omnetpp::endl;
//    }
//    for (int i = 0; i < thisNode->getNumInLinks(); ++i) {
//        EV << "InLink " << i << " is " << thisNode->getLinkIn(i)->getLocalGate()->getBaseName() << omnetpp::endl;
//    }
    // find and store next hops
    for (int i = 0; i < topo->getNumNodes(); i++) {
        if (topo->getNode(i) == thisNode)
            continue;  // skip ourselves
        topo->calculateUnweightedSingleShortestPathsTo(topo->getNode(i));

        if (thisNode->getNumPaths() == 0)
            continue;  // not connected

        omnetpp::cGate *parentModuleGate = thisNode->getPath(0)->getLocalGate();
        int gateIndex = parentModuleGate->getIndex();
        int serverId = topo->getNode(i)->getModule()->getIndex();
        rtable[serverId] = gateIndex;
        EV << "  towards server id: " << serverId << " gateIndex is " << gateIndex << omnetpp::endl;
    }
    delete topo;
}

void Routing::handleMessage(omnetpp::cMessage *msg)
{
    Task *task = omnetpp::check_and_cast<Task *>(msg);
    int destinationServerId = task->getDestinationServer();
    EV << "destination "<< destinationServerId << omnetpp::endl;
    if (destinationServerId == serverId) {
        EV << "send task to local dispatcher " << task->getName() << omnetpp::endl;
        send(task, "localOut");
        return;
    }

    RoutingTable::iterator it = rtable.find(destinationServerId);
    if (it == rtable.end()) {
        EV << "serverId:  " << destinationServerId << " unreachable, discarding packet " << task->getName() << omnetpp::endl;
        cancelAndDelete(task);
        return;
    }

    int outGateIndex = (*it).second;
    EV << "forwarding task " << task->getName() << " on gate index " << outGateIndex << omnetpp::endl;
    EV << gate("out", outGateIndex)->getNextGate()->getNextGate() << omnetpp::endl;
    EV << "outgate connect to server " << gate("out", outGateIndex)->getNextGate()->getNextGate()->getOwnerModule()->getIndex() << omnetpp::endl;
    task->setHopCount(task->getHopCount()+1);
    intVector& hopPathForUpdate = task->getHopPathForUpdate();
    hopPathForUpdate.push_back(serverId);
    send(task, "out", outGateIndex);
}

