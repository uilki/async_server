#ifndef __E91MRJHFEQGLMMPZZ361AGAJII8S6DR13FEOL5BUHPKKBVSKZ9275NLUXGSYKWFQET8KD9O5I__H__
#define __E91MRJHFEQGLMMPZZ361AGAJII8S6DR13FEOL5BUHPKKBVSKZ9275NLUXGSYKWFQET8KD9O5I__H__

#include "executor.h"
#include "threadpool.h"

#include <boost/asio/ip/tcp.hpp>

namespace net {
class Server :  public std::enable_shared_from_this<Server>
{
    enum {backlog = 10 };
public:
    using RequestHandler = std::function<std::string(std::string)>;

    Server(Executor& executor, const boost::asio::ip::tcp& protocol, RequestHandler handler);

    boost::system::error_code start(const boost::asio::ip::tcp::endpoint &endpoint);

    void stop();
private:
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    boost::system::error_code bind(const boost::asio::ip::tcp::endpoint& endpoint);
    boost::system::error_code listen();
    void startAccept();
    void onAccept(const std::error_code& err, boost::asio::ip::tcp::socket socket);

    boost::asio::io_service&       io_service_;
    StrandPtr                      strand_    ;
    boost::asio::ip::tcp::acceptor acceptor_  ;
    ThreadPool                     pool_      ;
    RequestHandler                 handler_   ;
};
} // namespace net

#endif // __E91MRJHFEQGLMMPZZ361AGAJII8S6DR13FEOL5BUHPKKBVSKZ9275NLUXGSYKWFQET8KD9O5I__H__
