/*
 * SSTSchedulingAlgo.h
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#ifndef ALGORITHM_SSTSCHEDULINGALGO_H_
#define ALGORITHM_SSTSCHEDULINGALGO_H_

#include "SchedulingAlgo.h"

// SST (Shortest Spare Time) : deadline - processing - currentTime
class SSTSchedulingAlgo: public SchedulingAlgo {
public:
    SSTSchedulingAlgo(TaskQueue *taskQueue);
    ~SSTSchedulingAlgo();
    void scheduling() override;
    void insertTaskIntoWaitingQueue(Task *task) override;
};

#endif /* ALGORITHM_SSTSCHEDULINGALGO_H_ */
