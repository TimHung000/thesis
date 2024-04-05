/*
 * ProposedDispatchingAlgo.h
 *
 *  Created on: Apr 3, 2024
 *      Author: tim
 */

#ifndef ALGORITHM_PROPOSEDDISPATCHINGALGO_H_
#define ALGORITHM_PROPOSEDDISPATCHINGALGO_H_
#include "DispatchingAlgo.h"
#include "Task_m.h"

class ProposedDispatchingAlgo: public DispatchingAlgo {
public:
    ProposedDispatchingAlgo(SchedulingAlgo *schedulingAlgo, TaskQueue *taskQueue);
    ~ProposedDispatchingAlgo();
    void execute(omnetpp::cMessage *msg) override;

protected:
    std::vector<Task*> preemptive(Task *task);
    void taskDispatching(Task *task);
    bool isNeighborLoadLessThanServerLoad();
    void taskOffloading(Task *task);
    void trySplitAndOffload(Task *task);
};


#endif /* ALGORITHM_PROPOSEDDISPATCHINGALGO_H_ */
