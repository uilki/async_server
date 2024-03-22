#ifndef __V0TV0MIW0046UCMDUQZT3JIYNPAIO6EB4O38ZAQF2U77T3KISIGMV54LRCYTD4FD5O4NDBEB3__H__
#define __V0TV0MIW0046UCMDUQZT3JIYNPAIO6EB4O38ZAQF2U77T3KISIGMV54LRCYTD4FD5O4NDBEB3__H__

#include "threadpool.h"
#include "strand.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ssl.hpp>

using Socket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;

namespace net::server {

class Connection: public std::enable_shared_from_this<Connection>
{
public:
    using RequestHandler = std::function<std::string(std::string, bool&)>;

    explicit Connection(Socket s, StrandPtr strand, ThreadPool& p, RequestHandler &handler);
    ~Connection();

    void start();

private:
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    void handleHandshake(const boost::system::error_code& error);
    void onTimeout(const std::error_code& ec);
    bool writeAsync(std::string str);
    void onRead(const std::error_code& ec, std::uint32_t sz);
    void closeConnection(const std::string& mes);

    Socket                    s_    ;
    boost::asio::streambuf    b_    ;
    boost::asio::steady_timer timer_;

    StrandPtr       strand_ ;
    ThreadPool&     pool_   ;
    RequestHandler& handler_;
    bool            signed_ ;
    const std::chrono::steady_clock::duration timeout_  ;
};
using ConnectionPtr = std::shared_ptr<Connection>;
} // namespace net::server

#endif // __V0TV0MIW0046UCMDUQZT3JIYNPAIO6EB4O38ZAQF2U77T3KISIGMV54LRCYTD4FD5O4NDBEB3__H__
