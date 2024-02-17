//
// Generated file, do not edit! Created by opp_msgtool 6.0 from Task.msg.
//

#ifndef __TASK_M_H
#define __TASK_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// opp_msgtool version check
#define MSGC_VERSION 0x0600
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgtool: 'make clean' should help.
#endif

class Task;
// cplusplus {{
#include <vector>
#include "SubTask.h"
typedef std::vector<int> intVector;
typedef std::vector<SubTask*> subTaskVector;
// }}

/**
 * Class generated from <tt>Task.msg:23</tt> by opp_msgtool.
 * <pre>
 * //
 * // Task message definition for queueing framework
 * //
 * packet Task
 * {
 *     int64_t taskId;
 *     simtime_t creationTime;
 *     simtime_t totalWaitingTime; 	    // total time spent on waiting in queues
 *     simtime_t totalProcessingTime;      // total time spent in processing
 *     simtime_t totalPropagationTime;     // total time spent in propagation
 *     simtime_t finishedTime;
 *     simtime_t deadline;				 	// task need to finish before creattionTime + deadline
 *     double taskSize;				    // bytes
 *     double requiredCycle;
 *     double processedCycle;
 *     int arrivingServer;
 *     int runningServer;
 *     int destinationServer;
 *     int hopCount;
 *     bool isCompleted;
 *     bool fromDispatcher;
 *     intVector hopPath;
 * 
 * 
 *     // task can be divided into "totalSubTaskCount" subtasks, 
 *     // the task size and required cpu cycle of each subtask are given
 *     int totalSubTaskCount;
 *     subTaskVector subTaskVec;          // all subtasks that this packet contain      
 * 
 * }
 * </pre>
 */
class Task : public ::omnetpp::cPacket
{
  protected:
    int64_t taskId = 0;
    omnetpp::simtime_t creationTime = SIMTIME_ZERO;
    omnetpp::simtime_t totalWaitingTime = SIMTIME_ZERO;
    omnetpp::simtime_t totalProcessingTime = SIMTIME_ZERO;
    omnetpp::simtime_t totalPropagationTime = SIMTIME_ZERO;
    omnetpp::simtime_t finishedTime = SIMTIME_ZERO;
    omnetpp::simtime_t deadline = SIMTIME_ZERO;
    double taskSize = 0;
    double requiredCycle = 0;
    double processedCycle = 0;
    int arrivingServer = 0;
    int runningServer = 0;
    int destinationServer = 0;
    int hopCount = 0;
    bool isCompleted_ = false;
    bool fromDispatcher = false;
    intVector hopPath;
    int totalSubTaskCount = 0;
    subTaskVector subTaskVec;

  private:
    void copy(const Task& other);

  protected:
    bool operator==(const Task&) = delete;

  public:
    Task(const char *name=nullptr, short kind=0);
    Task(const Task& other);
    virtual ~Task();
    Task& operator=(const Task& other);
    virtual Task *dup() const override {return new Task(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual int64_t getTaskId() const;
    virtual void setTaskId(int64_t taskId);

    virtual omnetpp::simtime_t getCreationTime() const;
    virtual void setCreationTime(omnetpp::simtime_t creationTime);

    virtual omnetpp::simtime_t getTotalWaitingTime() const;
    virtual void setTotalWaitingTime(omnetpp::simtime_t totalWaitingTime);

    virtual omnetpp::simtime_t getTotalProcessingTime() const;
    virtual void setTotalProcessingTime(omnetpp::simtime_t totalProcessingTime);

    virtual omnetpp::simtime_t getTotalPropagationTime() const;
    virtual void setTotalPropagationTime(omnetpp::simtime_t totalPropagationTime);

    virtual omnetpp::simtime_t getFinishedTime() const;
    virtual void setFinishedTime(omnetpp::simtime_t finishedTime);

    virtual omnetpp::simtime_t getDeadline() const;
    virtual void setDeadline(omnetpp::simtime_t deadline);

    virtual double getTaskSize() const;
    virtual void setTaskSize(double taskSize);

    virtual double getRequiredCycle() const;
    virtual void setRequiredCycle(double requiredCycle);

    virtual double getProcessedCycle() const;
    virtual void setProcessedCycle(double processedCycle);

    virtual int getArrivingServer() const;
    virtual void setArrivingServer(int arrivingServer);

    virtual int getRunningServer() const;
    virtual void setRunningServer(int runningServer);

    virtual int getDestinationServer() const;
    virtual void setDestinationServer(int destinationServer);

    virtual int getHopCount() const;
    virtual void setHopCount(int hopCount);

    virtual bool isCompleted() const;
    virtual void setIsCompleted(bool isCompleted);

    virtual bool getFromDispatcher() const;
    virtual void setFromDispatcher(bool fromDispatcher);

    virtual const intVector& getHopPath() const;
    virtual intVector& getHopPathForUpdate() { return const_cast<intVector&>(const_cast<Task*>(this)->getHopPath());}
    virtual void setHopPath(const intVector& hopPath);

    virtual int getTotalSubTaskCount() const;
    virtual void setTotalSubTaskCount(int totalSubTaskCount);

    virtual const subTaskVector& getSubTaskVec() const;
    virtual subTaskVector& getSubTaskVecForUpdate() { return const_cast<subTaskVector&>(const_cast<Task*>(this)->getSubTaskVec());}
    virtual void setSubTaskVec(const subTaskVector& subTaskVec);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Task& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Task& obj) {obj.parsimUnpack(b);}


namespace omnetpp {

inline any_ptr toAnyPtr(const intVector *p) {if (auto obj = as_cObject(p)) return any_ptr(obj); else return any_ptr(p);}
template<> inline intVector *fromAnyPtr(any_ptr ptr) { return ptr.get<intVector>(); }
inline any_ptr toAnyPtr(const subTaskVector *p) {if (auto obj = as_cObject(p)) return any_ptr(obj); else return any_ptr(p);}
template<> inline subTaskVector *fromAnyPtr(any_ptr ptr) { return ptr.get<subTaskVector>(); }
template<> inline Task *fromAnyPtr(any_ptr ptr) { return check_and_cast<Task*>(ptr.get<cObject>()); }

}  // namespace omnetpp

#endif // ifndef __TASK_M_H

