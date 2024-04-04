/*
 * SchedulingAlgo.h
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#ifndef ALGORITHM_SCHEDULINGALGO_H_
#define ALGORITHM_SCHEDULINGALGO_H_

#include "Task_m.h"

class TaskQueue;

class SchedulingAlgo {
public:
    virtual ~SchedulingAlgo() = default;
    virtual void scheduling() = 0;
    virtual void insertTaskIntoWaitingQueue(Task *task) = 0;
protected:
    TaskQueue *taskQueue;
};




#endif /* ALGORITHM_SCHEDULINGALGO_H_ */
