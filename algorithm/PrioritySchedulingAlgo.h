/*
 * PrioritySchedulingAlgo.h
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#ifndef ALGORITHM_PRIORITYSCHEDULINGALGO_H_
#define ALGORITHM_PRIORITYSCHEDULINGALGO_H_

#include "SchedulingAlgo.h"


class PrioritySchedulingAlgo: public SchedulingAlgo {
public:
    PrioritySchedulingAlgo(TaskQueue *taskQueue);
    ~PrioritySchedulingAlgo();
    void scheduling() override;
    std::list<Task*>::iterator insertTaskIntoWaitingQueue(Task *task) override;
    std::list<Task*>::iterator insertTaskIntoWaitingQueue(Task *task, std::list<Task*>::iterator it) override;
    std::list<Task*>::iterator getInsertionPoint(Task *task) override;
protected:
    double getPriority(double spareTime, double maxSpareTime, double wholeRequiredCycle, double maxWholeRequiredCycle, double taskSplitFactor);

};


#endif /* ALGORITHM_PRIORITYSCHEDULINGALGO_H_ */
