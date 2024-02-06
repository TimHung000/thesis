/*
 * subtask.h
 *
 *  Created on: Feb 6, 2024
 *      Author: tim
 */

#ifndef SUBTASK_H_
#define SUBTASK_H_

class SubTask
{
private:
    int subTaskId;
    double subTaskSize;
    double subTaskRequiredCPUCycle;

public:
    SubTask(int subTaskId, double subTaskSize, double subTaskRequiredCPUCycle);
    SubTask(SubTask *subTask);
    int getSubTaskId() const;
    double getSubTaskSize() const;
    double getSubTaskRequiredCPUCycle() const;
};

#endif /* SUBTASK_H_ */
