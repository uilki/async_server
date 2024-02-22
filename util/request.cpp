#include "request.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/schema.h>
#include <rapidjson/error/en.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace rapidjson;

namespace {
enum  Values {
    id   = 0,
    name = 1
};

const std::string schema { R"(
{
    "type": "object",
    "properties": {
        "transactionId": {
            "type": "integer",
            "minimum": 0
        },
        "functionName": {
            "type": "string"
        },
        "other": {
            "type": "array",
            "items": {
                "type": "string"
            },
            "minItems": 1

        }
    },
    "required": ["transactionId", "functionName"]
}
)"
};

const char* requiredValues[] {
    "transactionId",
    "functionName"
};

const char* other {"other"};
} // namespace

namespace util {
RequestParser::RequestParser()
{
    if (schemaDoc_.Parse(schema.c_str()).HasParseError()) {
        std::string schemaMessage{"Bad schema: "};
        throw BadRequest{(schemaMessage + GetParseError_En(schemaDoc_.GetParseError())).c_str()};
    }
}

std::string RequestParser::fromString(int transactionID, std::string name, std::string other) const
{

    Document request;
    request.SetObject();
    auto& alloc = request.GetAllocator();
    for (const auto& valName : requiredValues)
        { request.AddMember(Value(valName, alloc).Move(), {}, alloc); }

    request[requiredValues[Values::id]].SetInt64(transactionID);
    request[requiredValues[Values::name]].SetString(name.c_str(), alloc);

    auto valueList = [](std::string str) {
        boost::trim(str);
        std::list<std::string> result;
        boost::split(result, str, boost::is_space(), boost::token_compress_on);
        return result;
    }(std::move(other));

    if (!valueList.empty()) {
        Value values(Type::kArrayType);
        for (const auto& value : valueList)
            { values.PushBack(Value(value.c_str(), alloc).Move(), alloc); }
        request.AddMember(Value(::other, alloc).Move(), values, alloc);
    }

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    request.Accept(writer);

    return std::string{buffer.GetString()} + '\n';
}

RequestParser::Request RequestParser::parse(const char *json) const
{
    SchemaDocument schema{schemaDoc_};
    Document d;

    if (d.Parse(json).HasParseError())
        { throw BadRequest(GetParseError_En(d.GetParseError())); }

    SchemaValidator validator(schema);
    if (!d.Accept(validator))
        { throw BadRequest("Request does not match schema"); }

    Request result;
    result.id   = d[requiredValues[Values::id]]  .GetInt64();
    result.name = d[requiredValues[Values::name]].GetString();

    if (!d.HasMember(other))
        { return result; }

    for (const auto& value: d[other].GetArray())
        { result.args.push_back(value.GetString()); }

    return result;
}
} // namespace util
