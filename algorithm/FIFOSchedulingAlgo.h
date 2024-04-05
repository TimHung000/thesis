/*
 * FIFOSchedulingAlgo.h
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#ifndef ALGORITHM_FIFOSCHEDULINGALGO_H_
#define ALGORITHM_FIFOSCHEDULINGALGO_H_

#include "SchedulingAlgo.h"

// First-In First-Out
class FIFOSchedulingAlgo: public SchedulingAlgo {
public:
    FIFOSchedulingAlgo(TaskQueue *taskQueue);
    ~FIFOSchedulingAlgo();
    void scheduling() override;
    std::list<Task*>::iterator insertTaskIntoWaitingQueue(Task *task) override;
    std::list<Task*>::iterator insertTaskIntoWaitingQueue(Task *task, std::list<Task*>::iterator it) override;
    std::list<Task*>::iterator getInsertionPoint(Task *task) override;
};


#endif /* ALGORITHM_FIFOSCHEDULINGALGO_H_ */
