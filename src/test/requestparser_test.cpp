#include "../util/requestparser.h"
#include "../common/request.h"
#include "gtest/gtest.h"


const auto jsonSessionRequest = R"({"transactionId":1,"functionName":"foo","other":["first","second"]})";
const auto sampleSessionRequest = common::SessionRequest{1, "foo", {"first", "second"}};

const auto jsonAuthRequest = R"({"status":1,"other":["first","second"]})";
const auto sampleAuthRequest = common::AuthentificationRequest{1, {"first", "second"}};

namespace {

TEST(RequestParserSuite, Parse) {
    {
        util::RequestParser p{"requestSchema.json"};

        auto res = p.parse<common::SessionRequest, size_t, std::string, std::list<std::string>>(jsonSessionRequest);

        ASSERT_TRUE(res.id == sampleSessionRequest.id);
        ASSERT_TRUE(res.name == sampleSessionRequest.name);
        ASSERT_TRUE(res.args.size() == sampleSessionRequest.args.size()
                    && std::equal(res.args.begin(), res.args.end(), sampleSessionRequest.args.begin()));

    }
    {
        util::RequestParser p{"authRequestSchema.json"};

        auto res = p.parse<common::AuthentificationRequest, size_t, std::list<std::string>>(jsonAuthRequest);

        ASSERT_TRUE(res.id == sampleAuthRequest.id);
        ASSERT_TRUE(res.data.size() == sampleAuthRequest.data.size()
                    && std::equal(res.data.begin(), res.data.end(), sampleAuthRequest.data.begin()));

    }
}

TEST(RequestParserSuite, ToJson) {
    {
        util::RequestParser p{"requestSchema.json"};
        auto res = p.toJson(sampleSessionRequest.id, sampleSessionRequest.name, sampleSessionRequest.args);
        ASSERT_TRUE(res == jsonSessionRequest);
    }
    {
        util::RequestParser p{"authRequestSchema.json"};
        auto res = p.toJson(sampleAuthRequest.id, sampleAuthRequest.data);
        ASSERT_TRUE(res == jsonAuthRequest);
    }
}

}
