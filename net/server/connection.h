#ifndef __V0TV0MIW0046UCMDUQZT3JIYNPAIO6EB4O38ZAQF2U77T3KISIGMV54LRCYTD4FD5O4NDBEB3__H__
#define __V0TV0MIW0046UCMDUQZT3JIYNPAIO6EB4O38ZAQF2U77T3KISIGMV54LRCYTD4FD5O4NDBEB3__H__

#include "threadpool.h"
#include "messagepersistence.h"

#include <bitset>
#include <array>
#include <condition_variable>
#include <mutex>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>

namespace server {
class Connection: public std::enable_shared_from_this<Connection>
{
    enum { MAX_SIZE = 10 };
public:
    using RequestHandler = std::function<std::string(std::string)>;
    explicit Connection(boost::asio::ip::tcp::socket s, net::ThreadPool& p, RequestHandler &handler);

    void start();

private:
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    bool writeAsync(std::string str);
    void onRead(const std::error_code& ec, std::uint32_t sz);

    boost::asio::ip::tcp::socket s_;
    boost::asio::streambuf       b_;

    util::MessagePersistence<MAX_SIZE> messages_;
    net::ThreadPool&                   pool_    ;
    RequestHandler&                    handler_;
};
using ConnectionPtr = std::shared_ptr<Connection>;
} // namespace server

#endif // __V0TV0MIW0046UCMDUQZT3JIYNPAIO6EB4O38ZAQF2U77T3KISIGMV54LRCYTD4FD5O4NDBEB3__H__
