#include "../net/server/auth/authentificator.h"
#include "../net/server/auth/serverfabric.h"
#include "gtest/gtest.h"

#include <functional>
#include <filesystem>

namespace {
namespace fs = std::filesystem;

using Result = net::server::Authentificator::Result;

static constexpr std::pair<const char*, const char*> user[]={
    {"user1", "password1"},
    {"user2", "password2"},
    {"user3", "password3"},
};

class AuthentificatorTest : public testing::Test {
protected:
    net::server::Authentificator a{net::server::auth::PasswordServer::getPasswordServer(net::server::AbstractPasswordServer::Kind::file, "passwords")};
    std::hash<std::string> hasher_;
public:
    void cleanUp() const
    {
        if (fs::exists(fs::current_path().append("passwords")))
            fs::remove(fs::current_path().append("passwords"));
    }
};

TEST_F(AuthentificatorTest, SignUp) {
    cleanUp();
    for (size_t  i = 0; i < std::size(user) - 1; ++i) {
        EXPECT_TRUE(a.signUp(user[i].first, hasher_(user[i].second)));
        EXPECT_FALSE(a.signUp(user[i].first, hasher_(user[i].second)));
    }
}

TEST_F(AuthentificatorTest, SignIn) {
    EXPECT_EQ(Result::unregistered, a.sighIn(user[2].first,hasher_(user[2].second)));
    EXPECT_EQ(Result::signIn, a.sighIn(user[0].first,hasher_(user[0].second)));
    EXPECT_EQ(Result::wrongpassword, a.sighIn(user[1].first,hasher_(user[0].second)));
}

}  // namespace
