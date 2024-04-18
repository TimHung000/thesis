/*
 * DispatchingAlgo.h
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#ifndef DISPATCHINGALGO_H_
#define DISPATCHINGALGO_H_

#include <omnetpp.h>
#include "SchedulingAlgo.h"

class TaskQueue;
class DispatchingAlgo {
public:
    virtual ~DispatchingAlgo() = default;
    virtual void execute(omnetpp::cMessage *msg) = 0;
protected:
    SchedulingAlgo *schedulingAlgo;
    TaskQueue *taskQueue;
};

#endif /* DISPATCHINGALGO_H_ */
