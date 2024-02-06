//
// Generated file, do not edit! Created by opp_msgtool 6.0 from Info.msg.
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
#include "Info_m.h"

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

Register_Class(Info)

Info::Info(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

Info::Info(const Info& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

Info::~Info()
{
}

Info& Info::operator=(const Info& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void Info::copy(const Info& other)
{
    this->serverIdx = other.serverIdx;
    this->serverName = other.serverName;
    this->serverFrequency = other.serverFrequency;
    this->serverCapacity = other.serverCapacity;
    this->taskCount = other.taskCount;
    this->totalRequiredCycle = other.totalRequiredCycle;
    this->totalMemoryConsumed = other.totalMemoryConsumed;
    this->hopCount = other.hopCount;
    this->creationTime = other.creationTime;
}

void Info::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->serverIdx);
    doParsimPacking(b,this->serverName);
    doParsimPacking(b,this->serverFrequency);
    doParsimPacking(b,this->serverCapacity);
    doParsimPacking(b,this->taskCount);
    doParsimPacking(b,this->totalRequiredCycle);
    doParsimPacking(b,this->totalMemoryConsumed);
    doParsimPacking(b,this->hopCount);
    doParsimPacking(b,this->creationTime);
}

void Info::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->serverIdx);
    doParsimUnpacking(b,this->serverName);
    doParsimUnpacking(b,this->serverFrequency);
    doParsimUnpacking(b,this->serverCapacity);
    doParsimUnpacking(b,this->taskCount);
    doParsimUnpacking(b,this->totalRequiredCycle);
    doParsimUnpacking(b,this->totalMemoryConsumed);
    doParsimUnpacking(b,this->hopCount);
    doParsimUnpacking(b,this->creationTime);
}

int Info::getServerIdx() const
{
    return this->serverIdx;
}

void Info::setServerIdx(int serverIdx)
{
    this->serverIdx = serverIdx;
}

const char * Info::getServerName() const
{
    return this->serverName.c_str();
}

void Info::setServerName(const char * serverName)
{
    this->serverName = serverName;
}

double Info::getServerFrequency() const
{
    return this->serverFrequency;
}

void Info::setServerFrequency(double serverFrequency)
{
    this->serverFrequency = serverFrequency;
}

double Info::getServerCapacity() const
{
    return this->serverCapacity;
}

void Info::setServerCapacity(double serverCapacity)
{
    this->serverCapacity = serverCapacity;
}

int Info::getTaskCount() const
{
    return this->taskCount;
}

void Info::setTaskCount(int taskCount)
{
    this->taskCount = taskCount;
}

double Info::getTotalRequiredCycle() const
{
    return this->totalRequiredCycle;
}

void Info::setTotalRequiredCycle(double totalRequiredCycle)
{
    this->totalRequiredCycle = totalRequiredCycle;
}

double Info::getTotalMemoryConsumed() const
{
    return this->totalMemoryConsumed;
}

void Info::setTotalMemoryConsumed(double totalMemoryConsumed)
{
    this->totalMemoryConsumed = totalMemoryConsumed;
}

int Info::getHopCount() const
{
    return this->hopCount;
}

void Info::setHopCount(int hopCount)
{
    this->hopCount = hopCount;
}

omnetpp::simtime_t Info::getCreationTime() const
{
    return this->creationTime;
}

void Info::setCreationTime(omnetpp::simtime_t creationTime)
{
    this->creationTime = creationTime;
}

class InfoDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_serverIdx,
        FIELD_serverName,
        FIELD_serverFrequency,
        FIELD_serverCapacity,
        FIELD_taskCount,
        FIELD_totalRequiredCycle,
        FIELD_totalMemoryConsumed,
        FIELD_hopCount,
        FIELD_creationTime,
    };
  public:
    InfoDescriptor();
    virtual ~InfoDescriptor();

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

Register_ClassDescriptor(InfoDescriptor)

InfoDescriptor::InfoDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(Info)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

InfoDescriptor::~InfoDescriptor()
{
    delete[] propertyNames;
}

bool InfoDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Info *>(obj)!=nullptr;
}

const char **InfoDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *InfoDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int InfoDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 9+base->getFieldCount() : 9;
}

unsigned int InfoDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_serverIdx
        FD_ISEDITABLE,    // FIELD_serverName
        FD_ISEDITABLE,    // FIELD_serverFrequency
        FD_ISEDITABLE,    // FIELD_serverCapacity
        FD_ISEDITABLE,    // FIELD_taskCount
        FD_ISEDITABLE,    // FIELD_totalRequiredCycle
        FD_ISEDITABLE,    // FIELD_totalMemoryConsumed
        FD_ISEDITABLE,    // FIELD_hopCount
        FD_ISEDITABLE,    // FIELD_creationTime
    };
    return (field >= 0 && field < 9) ? fieldTypeFlags[field] : 0;
}

const char *InfoDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "serverIdx",
        "serverName",
        "serverFrequency",
        "serverCapacity",
        "taskCount",
        "totalRequiredCycle",
        "totalMemoryConsumed",
        "hopCount",
        "creationTime",
    };
    return (field >= 0 && field < 9) ? fieldNames[field] : nullptr;
}

int InfoDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "serverIdx") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "serverName") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "serverFrequency") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "serverCapacity") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "taskCount") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "totalRequiredCycle") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "totalMemoryConsumed") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "hopCount") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "creationTime") == 0) return baseIndex + 8;
    return base ? base->findField(fieldName) : -1;
}

const char *InfoDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_serverIdx
        "string",    // FIELD_serverName
        "double",    // FIELD_serverFrequency
        "double",    // FIELD_serverCapacity
        "int",    // FIELD_taskCount
        "double",    // FIELD_totalRequiredCycle
        "double",    // FIELD_totalMemoryConsumed
        "int",    // FIELD_hopCount
        "omnetpp::simtime_t",    // FIELD_creationTime
    };
    return (field >= 0 && field < 9) ? fieldTypeStrings[field] : nullptr;
}

const char **InfoDescriptor::getFieldPropertyNames(int field) const
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

const char *InfoDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int InfoDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    Info *pp = omnetpp::fromAnyPtr<Info>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void InfoDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    Info *pp = omnetpp::fromAnyPtr<Info>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'Info'", field);
    }
}

const char *InfoDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    Info *pp = omnetpp::fromAnyPtr<Info>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string InfoDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    Info *pp = omnetpp::fromAnyPtr<Info>(object); (void)pp;
    switch (field) {
        case FIELD_serverIdx: return long2string(pp->getServerIdx());
        case FIELD_serverName: return oppstring2string(pp->getServerName());
        case FIELD_serverFrequency: return double2string(pp->getServerFrequency());
        case FIELD_serverCapacity: return double2string(pp->getServerCapacity());
        case FIELD_taskCount: return long2string(pp->getTaskCount());
        case FIELD_totalRequiredCycle: return double2string(pp->getTotalRequiredCycle());
        case FIELD_totalMemoryConsumed: return double2string(pp->getTotalMemoryConsumed());
        case FIELD_hopCount: return long2string(pp->getHopCount());
        case FIELD_creationTime: return simtime2string(pp->getCreationTime());
        default: return "";
    }
}

void InfoDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Info *pp = omnetpp::fromAnyPtr<Info>(object); (void)pp;
    switch (field) {
        case FIELD_serverIdx: pp->setServerIdx(string2long(value)); break;
        case FIELD_serverName: pp->setServerName((value)); break;
        case FIELD_serverFrequency: pp->setServerFrequency(string2double(value)); break;
        case FIELD_serverCapacity: pp->setServerCapacity(string2double(value)); break;
        case FIELD_taskCount: pp->setTaskCount(string2long(value)); break;
        case FIELD_totalRequiredCycle: pp->setTotalRequiredCycle(string2double(value)); break;
        case FIELD_totalMemoryConsumed: pp->setTotalMemoryConsumed(string2double(value)); break;
        case FIELD_hopCount: pp->setHopCount(string2long(value)); break;
        case FIELD_creationTime: pp->setCreationTime(string2simtime(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Info'", field);
    }
}

omnetpp::cValue InfoDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    Info *pp = omnetpp::fromAnyPtr<Info>(object); (void)pp;
    switch (field) {
        case FIELD_serverIdx: return pp->getServerIdx();
        case FIELD_serverName: return pp->getServerName();
        case FIELD_serverFrequency: return pp->getServerFrequency();
        case FIELD_serverCapacity: return pp->getServerCapacity();
        case FIELD_taskCount: return pp->getTaskCount();
        case FIELD_totalRequiredCycle: return pp->getTotalRequiredCycle();
        case FIELD_totalMemoryConsumed: return pp->getTotalMemoryConsumed();
        case FIELD_hopCount: return pp->getHopCount();
        case FIELD_creationTime: return pp->getCreationTime().dbl();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'Info' as cValue -- field index out of range?", field);
    }
}

void InfoDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Info *pp = omnetpp::fromAnyPtr<Info>(object); (void)pp;
    switch (field) {
        case FIELD_serverIdx: pp->setServerIdx(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_serverName: pp->setServerName(value.stringValue()); break;
        case FIELD_serverFrequency: pp->setServerFrequency(value.doubleValue()); break;
        case FIELD_serverCapacity: pp->setServerCapacity(value.doubleValue()); break;
        case FIELD_taskCount: pp->setTaskCount(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_totalRequiredCycle: pp->setTotalRequiredCycle(value.doubleValue()); break;
        case FIELD_totalMemoryConsumed: pp->setTotalMemoryConsumed(value.doubleValue()); break;
        case FIELD_hopCount: pp->setHopCount(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_creationTime: pp->setCreationTime(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Info'", field);
    }
}

const char *InfoDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr InfoDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    Info *pp = omnetpp::fromAnyPtr<Info>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void InfoDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    Info *pp = omnetpp::fromAnyPtr<Info>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Info'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

