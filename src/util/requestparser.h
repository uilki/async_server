#ifndef __AOXV73F3PFCDPUK3OSAQITQK8T5XKFN2CDAFUPM24YK4LJ3Q7I6L31L1V4DDRHSJ2RMYV49DK__H__
#define __AOXV73F3PFCDPUK3OSAQITQK8T5XKFN2CDAFUPM24YK4LJ3Q7I6L31L1V4DDRHSJ2RMYV49DK__H__

#include "helpers.h"
#include "typelist.h"

#include <list>

#include <rapidjson/document.h>
#include <rapidjson/schema.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace util {
using namespace rapidjson;

class BadRequest : public std::exception {
    const char* mes_;
public:
    explicit BadRequest(const char* mes) : mes_{mes} {}

public:
    inline const char *what() const noexcept override
        { return mes_; }
};

template<typename... Ts>
using TypeList = util::details::TypeList<Ts...>;
using NullType = util::details::NullType;

// parse helpers
template<typename T>
std::enable_if_t<std::is_same_v<T, bool       >, T> getValue(Value& it) { return it.GetBool()  ; }
template<typename T>
std::enable_if_t<std::is_same_v<T, uint32_t   >, T> getValue(Value& it) { return it.GetUint()  ; }
template<typename T>
std::enable_if_t<std::is_same_v<T, int32_t    >, T> getValue(Value& it) { return it.GetInt()   ; }
template<typename T>
std::enable_if_t<std::is_same_v<T, uint64_t   >, T> getValue(Value& it) { return it.GetUint64(); }
template<typename T>
std::enable_if_t<std::is_same_v<T, int64_t    >, T> getValue(Value& it) { return it.GetInt64() ; }
template<typename T>
std::enable_if_t<std::is_same_v<T, float      >, T> getValue(Value& it)  { return it.GetFloat(); }
template<typename T>
std::enable_if_t<std::is_same_v<T, double     >, T> getValue(Value& it) { return it.GetDouble(); }
template<typename T>
std::enable_if_t<std::is_same_v<T, std::string>, T> getValue(Value& it) { return it.GetString(); }
template<typename T>
std::enable_if_t<details::is_array_like_v<T>   ,T> getValue(Value& it)
{
    T value;
    for (auto& v1: it.GetArray())
        { value.push_back(getValue<typename T::value_type>(v1)); }
    return value;
}

template <typename ReturnType, typename TList, typename... Values> struct Call;

template <typename ReturnType, typename... Values>
struct Call<ReturnType, NullType, Values...>
{
    static ReturnType call(Value::MemberIterator it, Values ... values)
        { return ReturnType(values...); }
};

template <typename ReturnType, typename List, typename... Values>
struct Call
{
    static ReturnType call(Value::MemberIterator it, Values ...values)
    {
        using value_t = details::remove_cv_ref_t<typename std::decay<typename List::Head>::type>;
        auto value = getValue<value_t>(it->value);
        return Call<ReturnType, typename List::Tail, Values..., value_t>::call(
            ++it, values..., value);
    }
};

// toJson helpers


template<typename T>
std::enable_if_t<std::is_arithmetic_v<details::remove_cv_ref_t<T>>, Value> toValue(T&& t, MemoryPoolAllocator<>& a)
    { return Value(t); }
template<typename T>
std::enable_if_t<std::is_enum_v<details::remove_cv_ref_t<T>>, Value> toValue(const T& t, MemoryPoolAllocator<>& a)
    { return toValue(static_cast<std::underlying_type_t<details::remove_cv_ref_t<T>>>(t), a); }

template<typename T>
std::enable_if_t<std::is_convertible_v<details::remove_cv_ref_t<T>, std::string>, Value> toValue(const T& t, MemoryPoolAllocator<>& a)
{
    std::string e{t};
    Value v;
    v.SetString(e.c_str(),e.length(), a);
    return v;
}

template<typename T>
std::enable_if_t<details::is_array_like_v<details::remove_cv_ref_t<T>>, Value> toValue(const T& t, MemoryPoolAllocator<>& a)
{
    Value v(kArrayType);
    for (const auto& e: t)
    { v.PushBack(toValue(e,a),a); }
    return v;
}

template<typename T>
void createValueList(std::list<Value>& l,MemoryPoolAllocator<>& a, const T& f)
    { l.push_back(toValue(f, a)); }

template<typename First, typename ... Rest>
void createValueList(std::list<Value>& l,MemoryPoolAllocator<>& a, const First& f, const Rest& ...r)
{
    l.push_back(toValue(f, a));
    createValueList(l,a,r...);
}

class RequestParser
{
public:    
    explicit RequestParser(const char *schema);

    template<typename ReturnType, typename... Types>
    ReturnType parse(const char* json)
    {
        using List = TypeList<Types...>;
        SchemaDocument schema{schemaDoc_};
        Document d;

        if (d.Parse(json).HasParseError())
            { throw BadRequest(GetParseError_En(d.GetParseError())); }

        SchemaValidator validator(schema);
        if (!d.Accept(validator))
            { throw BadRequest("Request does not match schema"); }

        return Call<ReturnType, List>::call(d.MemberBegin());
    }

    template<typename ... Ts>
    std::string toJson(const Ts& ...t)
    {
        auto propertiesMemberIterator = schemaDoc_.GetObject().FindMember("properties");
        if (propertiesMemberIterator == schemaDoc_.GetObject().MemberEnd())
            { throw BadRequest("No properties in schema"); }

        auto propertiesObject = propertiesMemberIterator->value.GetObject();
        if (propertiesObject.ObjectEmpty())
            { throw BadRequest("No properties in schema"); }

        Document request;
        request.SetObject();
        auto& alloc = request.GetAllocator();

        std::list<Value> l ;
        createValueList(l, alloc, t...);

        auto it = l.begin();
        for (const auto& member : propertiesObject)
        { if (it == l.end()) break;
            request.AddMember(Value(member.name, alloc).Move(), (*it++).Move(), alloc); }

        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        request.Accept(writer);

        return std::string{buffer.GetString()};
    }

private:
    Document schemaDoc_;
};
} // namespace util
#endif // __AOXV73F3PFCDPUK3OSAQITQK8T5XKFN2CDAFUPM24YK4LJ3Q7I6L31L1V4DDRHSJ2RMYV49DK__H__
