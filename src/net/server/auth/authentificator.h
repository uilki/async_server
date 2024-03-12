#ifndef __DQ8EORBXLW9I191O9VOM7XQUUYNSNFHKU2FQNZT5QGU3703C8FLJOBZ4MQ5BE5179Z8C2UMWD__H__
#define __DQ8EORBXLW9I191O9VOM7XQUUYNSNFHKU2FQNZT5QGU3703C8FLJOBZ4MQ5BE5179Z8C2UMWD__H__

#include "abstractpasswordserver.h"

#include <memory>

namespace net::server {

class Authentificator
{
public:
    enum class Result{ signIn , unregistered, wrongpassword};
    Authentificator(std::shared_ptr<AbstractPasswordServer> server);
    ~Authentificator() = default;

    bool signUp(const std::string& name, size_t hash);
    Result sighIn(const std::string& name, size_t hash) const;
private:
    std::shared_ptr<AbstractPasswordServer> server_;
};
}

#endif // __DQ8EORBXLW9I191O9VOM7XQUUYNSNFHKU2FQNZT5QGU3703C8FLJOBZ4MQ5BE5179Z8C2UMWD__H__
