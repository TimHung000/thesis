/*
 * GreedyDispatchingAlgo.h
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#ifndef ALGORITHM_GREEDYDISPATCHINGALGO_H_
#define ALGORITHM_GREEDYDISPATCHINGALGO_H_
#include "DispatchingAlgo.h"

class GreedyDispatchingAlgo: public DispatchingAlgo {
public:
    GreedyDispatchingAlgo(SchedulingAlgo *schedulingAlgo, TaskQueue *taskQueue);
    ~GreedyDispatchingAlgo();
    void execute(omnetpp::cMessage *msg) override;

};


#endif /* ALGORITHM_GREEDYDISPATCHINGALGO_H_ */
