#include "requestparser.h"

#include <fstream>
#include <iterator>

namespace util {
RequestParser::RequestParser(const char* schema)
{
    std::ifstream is{schema};
    if (!is.is_open())
        { throw BadRequest{"Can't open schema file"}; }

    std::string schemaData{std::istreambuf_iterator<char>(is),std::istreambuf_iterator<char>()};
    if (schemaData.empty()) {
        is.close();
        throw BadRequest{"Empty schema file"};
    }

    if (schemaDoc_.Parse(schemaData.c_str()).HasParseError()) {
        std::string schemaMessage{"Bad schema: "};
        throw BadRequest{(schemaMessage + GetParseError_En(schemaDoc_.GetParseError())).c_str()};
    }
}
} // namespace util
