/*
 * LocalDispatchingAlgo.h
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#ifndef ALGORITHM_LOCALDISPATCHINGALGO_H_
#define ALGORITHM_LOCALDISPATCHINGALGO_H_

#include "DispatchingAlgo.h"

class LocalDispatchingAlgo: public DispatchingAlgo {
public:
    LocalDispatchingAlgo(SchedulingAlgo *schedulingAlgo, TaskQueue *taskQueue);
    ~LocalDispatchingAlgo();
    void execute(omnetpp::cMessage *msg) override;
};



#endif /* ALGORITHM_LOCALDISPATCHINGALGO_H_ */
