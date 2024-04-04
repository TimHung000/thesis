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
    void insertTaskIntoWaitingQueue(Task *task) override;
};


#endif /* ALGORITHM_EDFSCHEDULINGALGO_H_ */
