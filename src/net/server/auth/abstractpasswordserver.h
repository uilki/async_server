#ifndef __FZXOR233PBBBL9LWETU9W8LI71Y1FNISEYMR4MVYYR9C85WSX4XMEW0T00Z6FCY7ZD58POD0I__H__
#define __FZXOR233PBBBL9LWETU9W8LI71Y1FNISEYMR4MVYYR9C85WSX4XMEW0T00Z6FCY7ZD58POD0I__H__

#include <memory>
#include <string>
namespace net::server {
class AbstractPasswordServer
{
public:
    enum class Kind { file, network };
    AbstractPasswordServer(){};
    virtual ~AbstractPasswordServer(){};

    virtual std::pair<bool, std::size_t>getUserHash(const std::string& user) = 0;
    virtual bool addUser(const std::string& user, std::size_t hash) = 0;
};

using PasswordServerPtr = std::shared_ptr<AbstractPasswordServer>;
} // namespace net::server

#endif // ABSTRACTPASSWORDSERVER_H
