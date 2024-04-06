/*
 * Utility.h
 *
 *  Created on: Apr 4, 2024
 *      Author: tim
 */

#ifndef ALGORITHM_UTILITY_H_
#define ALGORITHM_UTILITY_H_

#include "Task_m.h"


double getScore(double spareTime, double maxSpareTime, double wholeTask, double wholeRequiredCycle, double maxWholeRequiredCycle);
Task *createSubTask(Task *task, int subTaskIdx);


#endif /* ALGORITHM_UTILITY_H_ */
