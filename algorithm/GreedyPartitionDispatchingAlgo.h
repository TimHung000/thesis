/*
 * GreedyPartitionDispatching.h
 *
 *  Created on: Apr 4, 2024
 *      Author: tim
 */

#ifndef ALGORITHM_GREEDYPARTITIONDISPATCHINGALGO_H_
#define ALGORITHM_GREEDYPARTITIONDISPATCHINGALGO_H_

#include "DispatchingAlgo.h"

class GreedyPartitionDispatchingAlgo: public DispatchingAlgo {
public:
    GreedyPartitionDispatchingAlgo(SchedulingAlgo *schedulingAlgo, TaskQueue *taskQueue);
    ~GreedyPartitionDispatchingAlgo();
    void execute(omnetpp::cMessage *msg) override;

protected:
    void taskDispatching(Task *task);
    void splitAndOffload(Task *task);
};



#endif /* ALGORITHM_GREEDYPARTITIONDISPATCHINGALGO_H_ */
