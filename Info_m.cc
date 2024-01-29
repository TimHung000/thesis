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
    this->serverId = other.serverId;
    this->serverName = other.serverName;
    this->serverFrequency = other.serverFrequency;
    this->taskCount = other.taskCount;
    this->totalRequiredCycle = other.totalRequiredCycle;
}

void Info::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->serverId);
    doParsimPacking(b,this->serverName);
    doParsimPacking(b,this->serverFrequency);
    doParsimPacking(b,this->taskCount);
    doParsimPacking(b,this->totalRequiredCycle);
}

void Info::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->serverId);
    doParsimUnpacking(b,this->serverName);
    doParsimUnpacking(b,this->serverFrequency);
    doParsimUnpacking(b,this->taskCount);
    doParsimUnpacking(b,this->totalRequiredCycle);
}

int Info::getServerId() const
{
    return this->serverId;
}

void Info::setServerId(int serverId)
{
    this->serverId = serverId;
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

class InfoDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_serverId,
        FIELD_serverName,
        FIELD_serverFrequency,
        FIELD_taskCount,
        FIELD_totalRequiredCycle,
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
    return base ? 5+base->getFieldCount() : 5;
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
        FD_ISEDITABLE,    // FIELD_serverId
        FD_ISEDITABLE,    // FIELD_serverName
        FD_ISEDITABLE,    // FIELD_serverFrequency
        FD_ISEDITABLE,    // FIELD_taskCount
        FD_ISEDITABLE,    // FIELD_totalRequiredCycle
    };
    return (field >= 0 && field < 5) ? fieldTypeFlags[field] : 0;
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
        "serverId",
        "serverName",
        "serverFrequency",
        "taskCount",
        "totalRequiredCycle",
    };
    return (field >= 0 && field < 5) ? fieldNames[field] : nullptr;
}

int InfoDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "serverId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "serverName") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "serverFrequency") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "taskCount") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "totalRequiredCycle") == 0) return baseIndex + 4;
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
        "int",    // FIELD_serverId
        "string",    // FIELD_serverName
        "double",    // FIELD_serverFrequency
        "int",    // FIELD_taskCount
        "double",    // FIELD_totalRequiredCycle
    };
    return (field >= 0 && field < 5) ? fieldTypeStrings[field] : nullptr;
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
        case FIELD_serverId: return long2string(pp->getServerId());
        case FIELD_serverName: return oppstring2string(pp->getServerName());
        case FIELD_serverFrequency: return double2string(pp->getServerFrequency());
        case FIELD_taskCount: return long2string(pp->getTaskCount());
        case FIELD_totalRequiredCycle: return double2string(pp->getTotalRequiredCycle());
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
        case FIELD_serverId: pp->setServerId(string2long(value)); break;
        case FIELD_serverName: pp->setServerName((value)); break;
        case FIELD_serverFrequency: pp->setServerFrequency(string2double(value)); break;
        case FIELD_taskCount: pp->setTaskCount(string2long(value)); break;
        case FIELD_totalRequiredCycle: pp->setTotalRequiredCycle(string2double(value)); break;
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
        case FIELD_serverId: return pp->getServerId();
        case FIELD_serverName: return pp->getServerName();
        case FIELD_serverFrequency: return pp->getServerFrequency();
        case FIELD_taskCount: return pp->getTaskCount();
        case FIELD_totalRequiredCycle: return pp->getTotalRequiredCycle();
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
        case FIELD_serverId: pp->setServerId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_serverName: pp->setServerName(value.stringValue()); break;
        case FIELD_serverFrequency: pp->setServerFrequency(value.doubleValue()); break;
        case FIELD_taskCount: pp->setTaskCount(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_totalRequiredCycle: pp->setTotalRequiredCycle(value.doubleValue()); break;
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

