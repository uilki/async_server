#include "authentificator.h"

#include <string>

namespace net::server {
Authentificator::Authentificator(std::shared_ptr<AbstractPasswordServer> server)
    : server_{server}
{
}

bool Authentificator::signUp(const std::string &name, size_t hash)
{
    return server_->addUser(name, hash);
}

Authentificator::Result Authentificator::sighIn(const std::string &name, size_t hash) const
{
    auto [userExists, serverHash] = server_->getUserHash(name);
    if (!userExists)
        return Result::unregistered;

    return serverHash == hash ? Result::signIn : Result::wrongpassword;
}
} // namespace net::server
