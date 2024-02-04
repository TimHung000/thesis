//
// Generated file, do not edit! Created by opp_msgtool 6.0 from Info.msg.
//

#ifndef __INFO_M_H
#define __INFO_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// opp_msgtool version check
#define MSGC_VERSION 0x0600
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgtool: 'make clean' should help.
#endif

class Info;
/**
 * Class generated from <tt>Info.msg:17</tt> by opp_msgtool.
 * <pre>
 * message Info
 * {
 *     int serverIdx;
 *     string serverName;
 *     double serverFrequency;
 *     double serverCapacity;
 *     int taskCount;
 *     double totalRequiredCycle;
 *     double totalMemoryConsumed;
 * }
 * </pre>
 */
class Info : public ::omnetpp::cMessage
{
  protected:
    int serverIdx = 0;
    omnetpp::opp_string serverName;
    double serverFrequency = 0;
    double serverCapacity = 0;
    int taskCount = 0;
    double totalRequiredCycle = 0;
    double totalMemoryConsumed = 0;

  private:
    void copy(const Info& other);

  protected:
    bool operator==(const Info&) = delete;

  public:
    Info(const char *name=nullptr, short kind=0);
    Info(const Info& other);
    virtual ~Info();
    Info& operator=(const Info& other);
    virtual Info *dup() const override {return new Info(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual int getServerIdx() const;
    virtual void setServerIdx(int serverIdx);

    virtual const char * getServerName() const;
    virtual void setServerName(const char * serverName);

    virtual double getServerFrequency() const;
    virtual void setServerFrequency(double serverFrequency);

    virtual double getServerCapacity() const;
    virtual void setServerCapacity(double serverCapacity);

    virtual int getTaskCount() const;
    virtual void setTaskCount(int taskCount);

    virtual double getTotalRequiredCycle() const;
    virtual void setTotalRequiredCycle(double totalRequiredCycle);

    virtual double getTotalMemoryConsumed() const;
    virtual void setTotalMemoryConsumed(double totalMemoryConsumed);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Info& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Info& obj) {obj.parsimUnpack(b);}


namespace omnetpp {

template<> inline Info *fromAnyPtr(any_ptr ptr) { return check_and_cast<Info*>(ptr.get<cObject>()); }

}  // namespace omnetpp

#endif // ifndef __INFO_M_H

