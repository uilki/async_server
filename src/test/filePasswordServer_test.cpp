#include "../net/server/auth/filepasswordserver.h"
#include "gtest/gtest.h"

#include <filesystem>

namespace {
namespace fs = std::filesystem;
void cleanUp()
{
    if (fs::exists(fs::current_path().append("passwords")))
        { fs::remove(fs::current_path().append("passwords")); }
}

class FilePasswordServerTest : public testing::Test {
protected:
    std::unique_ptr<net::server::AbstractPasswordServer> server_{new net::server::FilePasswordServer("passwords")};
    std::hash<std::string> hasher_;
};

TEST(FilePasswordServerSuite, Constructor) {
    cleanUp();
    ASSERT_FALSE(fs::exists(fs::current_path().append("passwords")));
    {
        net::server::FilePasswordServer("passwords");
    }
    ASSERT_TRUE(fs::exists(fs::current_path().append("passwords")));
    fs::remove(fs::current_path().append("passwords"));
    cleanUp();
}

TEST_F(FilePasswordServerTest, UserAdd) {
    ASSERT_TRUE(server_->addUser("user1",  hasher_("password1")));
    ASSERT_TRUE(server_->addUser("user2",  hasher_("password2")));
    ASSERT_FALSE(server_->addUser("user1",  hasher_("password1")));
}

TEST_F(FilePasswordServerTest, GetHash) {
    auto res = std::make_pair( true, hasher_("password1"));
    ASSERT_TRUE(server_ != nullptr);
    ASSERT_EQ(server_->getUserHash("user1"), res);
    res = std::make_pair( false, 0);
    ASSERT_NE(server_->getUserHash("user2"), res);
    server_.reset(nullptr);
    cleanUp();
}

}  // namespace
