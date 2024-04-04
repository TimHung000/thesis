/*
 * RandomDispatchingAlgo.h
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#ifndef ALGORITHM_RANDOMDISPATCHINGALGO_H_
#define ALGORITHM_RANDOMDISPATCHINGALGO_H_
#include "DispatchingAlgo.h"

class RandomDispatchingAlgo: public DispatchingAlgo {
public:
    RandomDispatchingAlgo(SchedulingAlgo *schedulingAlgo, TaskQueue *taskQueue);
    ~RandomDispatchingAlgo();
    void execute(omnetpp::cMessage *msg) override;
};


#endif /* ALGORITHM_RANDOMDISPATCHINGALGO_H_ */
