#include "serverfabric.h"
#include "filepasswordserver.h"

namespace net::server::auth {
PasswordServerPtr auth::PasswordServer::getPasswordServer(AbstractPasswordServer::Kind kind, const std::string &path)
{
    switch (kind) {
    case AbstractPasswordServer::Kind::file   : return std::make_shared<FilePasswordServer>(path.c_str());
    case AbstractPasswordServer::Kind::network: return nullptr;
    default                                   : return nullptr;
    }
}
} // namespace net::server::auth
