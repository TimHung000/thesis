/*
 * EDFSchedulingAlgo.h
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#ifndef ALGORITHM_EDFSCHEDULINGALGO_H_
#define ALGORITHM_EDFSCHEDULINGALGO_H_

#include "SchedulingAlgo.h"

// Earliest Deadline First
class EDFSchedulingAlgo: public SchedulingAlgo {
public:
    EDFSchedulingAlgo(TaskQueue *taskQueue);
    ~EDFSchedulingAlgo();
    void scheduling() override;
    std::list<Task*>::iterator insertTaskIntoWaitingQueue(Task *task) override;
    std::list<Task*>::iterator insertTaskIntoWaitingQueue(Task *task, std::list<Task*>::iterator it) override;
    std::list<Task*>::iterator getInsertionPoint(Task *task) override;

};


#endif /* ALGORITHM_EDFSCHEDULINGALGO_H_ */
