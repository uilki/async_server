#ifndef __N1DGNHDZSQ53JF44EORQ2BUO9FNI4Z3473MLXM4H7BTMHNON0EUUPNLE56N133JX3J1HK7LTW__H__
#define __N1DGNHDZSQ53JF44EORQ2BUO9FNI4Z3473MLXM4H7BTMHNON0EUUPNLE56N133JX3J1HK7LTW__H__

#include "abstractpasswordserver.h"

#include <map>

namespace net::server {
class FilePasswordServer : public AbstractPasswordServer
{
public:
    explicit FilePasswordServer(const char* file);
    ~FilePasswordServer() override;

public:
    virtual std::pair<bool, std::size_t> getUserHash(const std::string &user) override;
    virtual bool addUser(const std::string &user, std::size_t hash) override;

private:
    void saveEntries();

    const std::string                  fileName_;
    std::map<std::string, std::size_t> users_   ;
};

} // namespace net::server

#endif // __N1DGNHDZSQ53JF44EORQ2BUO9FNI4Z3473MLXM4H7BTMHNON0EUUPNLE56N133JX3J1HK7LTW__H__
