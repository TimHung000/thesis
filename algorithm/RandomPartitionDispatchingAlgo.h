/*
 * RandomPartitionDispatching.h
 *
 *  Created on: Apr 4, 2024
 *      Author: tim
 */

#ifndef ALGORITHM_RANDOMPARTITIONDISPATCHINGALGO_H_
#define ALGORITHM_RANDOMPARTITIONDISPATCHINGALGO_H_

#include "DispatchingAlgo.h"

class RandomPartitionDispatchingAlgo: public DispatchingAlgo {
public:
    RandomPartitionDispatchingAlgo(SchedulingAlgo *schedulingAlgo, TaskQueue *taskQueue);
    ~RandomPartitionDispatchingAlgo();
    void execute(omnetpp::cMessage *msg) override;

protected:
    void taskDispatching(Task *task);
    void splitAndOffload(Task *task);
};



#endif /* ALGORITHM_RANDOMPARTITIONDISPATCHINGALGO_H_ */
