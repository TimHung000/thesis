/*
 * D2FODispatchingAlgo.h
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#ifndef ALGORITHM_D2FODISPATCHINGALGO_H_
#define ALGORITHM_D2FODISPATCHINGALGO_H_
#include "DispatchingAlgo.h"

class D2FODispatchingAlgo: public DispatchingAlgo {
public:
    D2FODispatchingAlgo(SchedulingAlgo *schedulingAlgo, TaskQueue *taskQueue);
    ~D2FODispatchingAlgo();
    void execute(omnetpp::cMessage *msg) override;

};


#endif /* ALGORITHM_D2FODISPATCHINGALGO_H_ */
