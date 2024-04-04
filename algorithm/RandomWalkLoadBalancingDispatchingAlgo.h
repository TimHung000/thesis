/*
 * RandomWalkLoadBalancingDispatchingAlgo.h
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#ifndef ALGORITHM_RANDOMWALKLOADBALANCINGDISPATCHINGALGO_H_
#define ALGORITHM_RANDOMWALKLOADBALANCINGDISPATCHINGALGO_H_
#include "DispatchingAlgo.h"

class RandomWalkLoadBalancingDispatchingAlgo: public DispatchingAlgo {
public:
    RandomWalkLoadBalancingDispatchingAlgo(SchedulingAlgo *schedulingAlgo, TaskQueue *taskQueue);
    ~RandomWalkLoadBalancingDispatchingAlgo();
    void execute(omnetpp::cMessage *msg) override;
protected:
    const int maximumHop;
    const double threshold;
};


#endif /* ALGORITHM_RANDOMWALKLOADBALANCINGDISPATCHINGALGO_H_ */
