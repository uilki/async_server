#ifndef __V0TV0MIW0046UCMDUQZT3JIYNPAIO6EB4O38ZAQF2U77T3KISIGMV54LRCYTD4FD5O4NDBEB3__H__
#define __V0TV0MIW0046UCMDUQZT3JIYNPAIO6EB4O38ZAQF2U77T3KISIGMV54LRCYTD4FD5O4NDBEB3__H__

#include "threadpool.h"
#include "strand.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>

namespace net::server {
class Connection: public std::enable_shared_from_this<Connection>
{
public:
    using RequestHandler = std::function<std::string(std::string, bool&)>;
    explicit Connection(boost::asio::ip::tcp::socket s, net::StrandPtr strand, net::ThreadPool& p, RequestHandler &handler);

    void start();

private:
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    bool writeAsync(std::string str);
    void onRead(const std::error_code& ec, std::uint32_t sz);
    void closeConnection(const std::string& mes);
    void onTimeout(const std::error_code& ec);
    boost::asio::ip::tcp::socket s_;
    boost::asio::streambuf       b_;

    net::StrandPtr strand_;

    boost::asio::steady_timer timer_;

    const std::chrono::steady_clock::duration timeout_  ;

    net::ThreadPool&                   pool_   ;
    RequestHandler&                    handler_;
    bool signed_;
};
using ConnectionPtr = std::shared_ptr<Connection>;
} // namespace net::server

#endif // __V0TV0MIW0046UCMDUQZT3JIYNPAIO6EB4O38ZAQF2U77T3KISIGMV54LRCYTD4FD5O4NDBEB3__H__
