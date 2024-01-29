//
// Generated file, do not edit! Created by opp_msgtool 6.0 from Task.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "Task_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

class StringVectorDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
    };
  public:
    StringVectorDescriptor();
    virtual ~StringVectorDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(StringVectorDescriptor)

StringVectorDescriptor::StringVectorDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(StringVector)), "")
{
    propertyNames = nullptr;
}

StringVectorDescriptor::~StringVectorDescriptor()
{
    delete[] propertyNames;
}

bool StringVectorDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<StringVector *>(obj)!=nullptr;
}

const char **StringVectorDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = { "existingClass",  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *StringVectorDescriptor::getProperty(const char *propertyName) const
{
    if (!strcmp(propertyName, "existingClass")) return "";
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int StringVectorDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 0+base->getFieldCount() : 0;
}

unsigned int StringVectorDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    return 0;
}

const char *StringVectorDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    return nullptr;
}

int StringVectorDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->findField(fieldName) : -1;
}

const char *StringVectorDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    return nullptr;
}

const char **StringVectorDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *StringVectorDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int StringVectorDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    StringVector *pp = omnetpp::fromAnyPtr<StringVector>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void StringVectorDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    StringVector *pp = omnetpp::fromAnyPtr<StringVector>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'StringVector'", field);
    }
}

const char *StringVectorDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    StringVector *pp = omnetpp::fromAnyPtr<StringVector>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string StringVectorDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    StringVector *pp = omnetpp::fromAnyPtr<StringVector>(object); (void)pp;
    switch (field) {
        default: return "";
    }
}

void StringVectorDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    StringVector *pp = omnetpp::fromAnyPtr<StringVector>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'StringVector'", field);
    }
}

omnetpp::cValue StringVectorDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    StringVector *pp = omnetpp::fromAnyPtr<StringVector>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'StringVector' as cValue -- field index out of range?", field);
    }
}

void StringVectorDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    StringVector *pp = omnetpp::fromAnyPtr<StringVector>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'StringVector'", field);
    }
}

const char *StringVectorDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    return nullptr;
}

omnetpp::any_ptr StringVectorDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    StringVector *pp = omnetpp::fromAnyPtr<StringVector>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void StringVectorDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    StringVector *pp = omnetpp::fromAnyPtr<StringVector>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'StringVector'", field);
    }
}

Register_Class(Task)

Task::Task(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

Task::Task(const Task& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

Task::~Task()
{
}

Task& Task::operator=(const Task& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void Task::copy(const Task& other)
{
    this->creationTime = other.creationTime;
    this->totalWaitingTime = other.totalWaitingTime;
    this->totalProcessingTime = other.totalProcessingTime;
    this->totalPropagationTime = other.totalPropagationTime;
    this->deadline = other.deadline;
    this->taskSize = other.taskSize;
    this->cpuCycles = other.cpuCycles;
    this->processedCycles = other.processedCycles;
    this->arrivingServer = other.arrivingServer;
    this->runningServer = other.runningServer;
    this->hopPath = other.hopPath;
}

void Task::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->creationTime);
    doParsimPacking(b,this->totalWaitingTime);
    doParsimPacking(b,this->totalProcessingTime);
    doParsimPacking(b,this->totalPropagationTime);
    doParsimPacking(b,this->deadline);
    doParsimPacking(b,this->taskSize);
    doParsimPacking(b,this->cpuCycles);
    doParsimPacking(b,this->processedCycles);
    doParsimPacking(b,this->arrivingServer);
    doParsimPacking(b,this->runningServer);
    doParsimPacking(b,this->hopPath);
}

void Task::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->creationTime);
    doParsimUnpacking(b,this->totalWaitingTime);
    doParsimUnpacking(b,this->totalProcessingTime);
    doParsimUnpacking(b,this->totalPropagationTime);
    doParsimUnpacking(b,this->deadline);
    doParsimUnpacking(b,this->taskSize);
    doParsimUnpacking(b,this->cpuCycles);
    doParsimUnpacking(b,this->processedCycles);
    doParsimUnpacking(b,this->arrivingServer);
    doParsimUnpacking(b,this->runningServer);
    doParsimUnpacking(b,this->hopPath);
}

omnetpp::simtime_t Task::getCreationTime() const
{
    return this->creationTime;
}

void Task::setCreationTime(omnetpp::simtime_t creationTime)
{
    this->creationTime = creationTime;
}

omnetpp::simtime_t Task::getTotalWaitingTime() const
{
    return this->totalWaitingTime;
}

void Task::setTotalWaitingTime(omnetpp::simtime_t totalWaitingTime)
{
    this->totalWaitingTime = totalWaitingTime;
}

omnetpp::simtime_t Task::getTotalProcessingTime() const
{
    return this->totalProcessingTime;
}

void Task::setTotalProcessingTime(omnetpp::simtime_t totalProcessingTime)
{
    this->totalProcessingTime = totalProcessingTime;
}

omnetpp::simtime_t Task::getTotalPropagationTime() const
{
    return this->totalPropagationTime;
}

void Task::setTotalPropagationTime(omnetpp::simtime_t totalPropagationTime)
{
    this->totalPropagationTime = totalPropagationTime;
}

omnetpp::simtime_t Task::getDeadline() const
{
    return this->deadline;
}

void Task::setDeadline(omnetpp::simtime_t deadline)
{
    this->deadline = deadline;
}

double Task::getTaskSize() const
{
    return this->taskSize;
}

void Task::setTaskSize(double taskSize)
{
    this->taskSize = taskSize;
}

double Task::getCpuCycles() const
{
    return this->cpuCycles;
}

void Task::setCpuCycles(double cpuCycles)
{
    this->cpuCycles = cpuCycles;
}

double Task::getProcessedCycles() const
{
    return this->processedCycles;
}

void Task::setProcessedCycles(double processedCycles)
{
    this->processedCycles = processedCycles;
}

int Task::getArrivingServer() const
{
    return this->arrivingServer;
}

void Task::setArrivingServer(int arrivingServer)
{
    this->arrivingServer = arrivingServer;
}

int Task::getRunningServer() const
{
    return this->runningServer;
}

void Task::setRunningServer(int runningServer)
{
    this->runningServer = runningServer;
}

const StringVector& Task::getHopPath() const
{
    return this->hopPath;
}

void Task::setHopPath(const StringVector& hopPath)
{
    this->hopPath = hopPath;
}

class TaskDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_creationTime,
        FIELD_totalWaitingTime,
        FIELD_totalProcessingTime,
        FIELD_totalPropagationTime,
        FIELD_deadline,
        FIELD_taskSize,
        FIELD_cpuCycles,
        FIELD_processedCycles,
        FIELD_arrivingServer,
        FIELD_runningServer,
        FIELD_hopPath,
    };
  public:
    TaskDescriptor();
    virtual ~TaskDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(TaskDescriptor)

TaskDescriptor::TaskDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(Task)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

TaskDescriptor::~TaskDescriptor()
{
    delete[] propertyNames;
}

bool TaskDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Task *>(obj)!=nullptr;
}

const char **TaskDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *TaskDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int TaskDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 11+base->getFieldCount() : 11;
}

unsigned int TaskDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_creationTime
        FD_ISEDITABLE,    // FIELD_totalWaitingTime
        FD_ISEDITABLE,    // FIELD_totalProcessingTime
        FD_ISEDITABLE,    // FIELD_totalPropagationTime
        FD_ISEDITABLE,    // FIELD_deadline
        FD_ISEDITABLE,    // FIELD_taskSize
        FD_ISEDITABLE,    // FIELD_cpuCycles
        FD_ISEDITABLE,    // FIELD_processedCycles
        FD_ISEDITABLE,    // FIELD_arrivingServer
        FD_ISEDITABLE,    // FIELD_runningServer
        FD_ISCOMPOUND,    // FIELD_hopPath
    };
    return (field >= 0 && field < 11) ? fieldTypeFlags[field] : 0;
}

const char *TaskDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "creationTime",
        "totalWaitingTime",
        "totalProcessingTime",
        "totalPropagationTime",
        "deadline",
        "taskSize",
        "cpuCycles",
        "processedCycles",
        "arrivingServer",
        "runningServer",
        "hopPath",
    };
    return (field >= 0 && field < 11) ? fieldNames[field] : nullptr;
}

int TaskDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "creationTime") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "totalWaitingTime") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "totalProcessingTime") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "totalPropagationTime") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "deadline") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "taskSize") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "cpuCycles") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "processedCycles") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "arrivingServer") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "runningServer") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "hopPath") == 0) return baseIndex + 10;
    return base ? base->findField(fieldName) : -1;
}

const char *TaskDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "omnetpp::simtime_t",    // FIELD_creationTime
        "omnetpp::simtime_t",    // FIELD_totalWaitingTime
        "omnetpp::simtime_t",    // FIELD_totalProcessingTime
        "omnetpp::simtime_t",    // FIELD_totalPropagationTime
        "omnetpp::simtime_t",    // FIELD_deadline
        "double",    // FIELD_taskSize
        "double",    // FIELD_cpuCycles
        "double",    // FIELD_processedCycles
        "int",    // FIELD_arrivingServer
        "int",    // FIELD_runningServer
        "StringVector",    // FIELD_hopPath
    };
    return (field >= 0 && field < 11) ? fieldTypeStrings[field] : nullptr;
}

const char **TaskDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *TaskDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int TaskDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    Task *pp = omnetpp::fromAnyPtr<Task>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void TaskDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    Task *pp = omnetpp::fromAnyPtr<Task>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'Task'", field);
    }
}

const char *TaskDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    Task *pp = omnetpp::fromAnyPtr<Task>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string TaskDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    Task *pp = omnetpp::fromAnyPtr<Task>(object); (void)pp;
    switch (field) {
        case FIELD_creationTime: return simtime2string(pp->getCreationTime());
        case FIELD_totalWaitingTime: return simtime2string(pp->getTotalWaitingTime());
        case FIELD_totalProcessingTime: return simtime2string(pp->getTotalProcessingTime());
        case FIELD_totalPropagationTime: return simtime2string(pp->getTotalPropagationTime());
        case FIELD_deadline: return simtime2string(pp->getDeadline());
        case FIELD_taskSize: return double2string(pp->getTaskSize());
        case FIELD_cpuCycles: return double2string(pp->getCpuCycles());
        case FIELD_processedCycles: return double2string(pp->getProcessedCycles());
        case FIELD_arrivingServer: return long2string(pp->getArrivingServer());
        case FIELD_runningServer: return long2string(pp->getRunningServer());
        case FIELD_hopPath: return "";
        default: return "";
    }
}

void TaskDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Task *pp = omnetpp::fromAnyPtr<Task>(object); (void)pp;
    switch (field) {
        case FIELD_creationTime: pp->setCreationTime(string2simtime(value)); break;
        case FIELD_totalWaitingTime: pp->setTotalWaitingTime(string2simtime(value)); break;
        case FIELD_totalProcessingTime: pp->setTotalProcessingTime(string2simtime(value)); break;
        case FIELD_totalPropagationTime: pp->setTotalPropagationTime(string2simtime(value)); break;
        case FIELD_deadline: pp->setDeadline(string2simtime(value)); break;
        case FIELD_taskSize: pp->setTaskSize(string2double(value)); break;
        case FIELD_cpuCycles: pp->setCpuCycles(string2double(value)); break;
        case FIELD_processedCycles: pp->setProcessedCycles(string2double(value)); break;
        case FIELD_arrivingServer: pp->setArrivingServer(string2long(value)); break;
        case FIELD_runningServer: pp->setRunningServer(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Task'", field);
    }
}

omnetpp::cValue TaskDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    Task *pp = omnetpp::fromAnyPtr<Task>(object); (void)pp;
    switch (field) {
        case FIELD_creationTime: return pp->getCreationTime().dbl();
        case FIELD_totalWaitingTime: return pp->getTotalWaitingTime().dbl();
        case FIELD_totalProcessingTime: return pp->getTotalProcessingTime().dbl();
        case FIELD_totalPropagationTime: return pp->getTotalPropagationTime().dbl();
        case FIELD_deadline: return pp->getDeadline().dbl();
        case FIELD_taskSize: return pp->getTaskSize();
        case FIELD_cpuCycles: return pp->getCpuCycles();
        case FIELD_processedCycles: return pp->getProcessedCycles();
        case FIELD_arrivingServer: return pp->getArrivingServer();
        case FIELD_runningServer: return pp->getRunningServer();
        case FIELD_hopPath: return omnetpp::toAnyPtr(&pp->getHopPath()); break;
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'Task' as cValue -- field index out of range?", field);
    }
}

void TaskDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Task *pp = omnetpp::fromAnyPtr<Task>(object); (void)pp;
    switch (field) {
        case FIELD_creationTime: pp->setCreationTime(value.doubleValue()); break;
        case FIELD_totalWaitingTime: pp->setTotalWaitingTime(value.doubleValue()); break;
        case FIELD_totalProcessingTime: pp->setTotalProcessingTime(value.doubleValue()); break;
        case FIELD_totalPropagationTime: pp->setTotalPropagationTime(value.doubleValue()); break;
        case FIELD_deadline: pp->setDeadline(value.doubleValue()); break;
        case FIELD_taskSize: pp->setTaskSize(value.doubleValue()); break;
        case FIELD_cpuCycles: pp->setCpuCycles(value.doubleValue()); break;
        case FIELD_processedCycles: pp->setProcessedCycles(value.doubleValue()); break;
        case FIELD_arrivingServer: pp->setArrivingServer(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_runningServer: pp->setRunningServer(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Task'", field);
    }
}

const char *TaskDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_hopPath: return omnetpp::opp_typename(typeid(StringVector));
        default: return nullptr;
    };
}

omnetpp::any_ptr TaskDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    Task *pp = omnetpp::fromAnyPtr<Task>(object); (void)pp;
    switch (field) {
        case FIELD_hopPath: return omnetpp::toAnyPtr(&pp->getHopPath()); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void TaskDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    Task *pp = omnetpp::fromAnyPtr<Task>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Task'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

