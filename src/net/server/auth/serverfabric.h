#ifndef __SHM067N86S3BOUQTWO1YOSYKZRM9EUDEF1HAKXPUW91NK6HNDSH3Z9CSJZA4VCZJLRK8M2VZK__H__
#define __SHM067N86S3BOUQTWO1YOSYKZRM9EUDEF1HAKXPUW91NK6HNDSH3Z9CSJZA4VCZJLRK8M2VZK__H__

#include "abstractpasswordserver.h"

namespace net::server::auth {
class PasswordServer{
public:
    static PasswordServerPtr getPasswordServer(AbstractPasswordServer::Kind kind, const std::string& path);
};
} // namespace net::server::auth

#endif // __SHM067N86S3BOUQTWO1YOSYKZRM9EUDEF1HAKXPUW91NK6HNDSH3Z9CSJZA4VCZJLRK8M2VZK__H__
