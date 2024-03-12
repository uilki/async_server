#ifndef __W1WN67AALW42X198BRBOOP6LP2ZQFEK1GPPF4UDIDLDSHK71S7ABS911YK367EB5LL7YX3HOT__H__
#define __W1WN67AALW42X198BRBOOP6LP2ZQFEK1GPPF4UDIDLDSHK71S7ABS911YK367EB5LL7YX3HOT__H__

#include "executor.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

namespace net {
class Connector
{
public:
    using Callback  = std::function<void(boost::asio::ip::tcp::socket, boost::system::error_code)>;

    Connector(Executor&                           executor,
              boost::asio::ip::tcp::endpoint      ep,
              std::chrono::steady_clock::duration timeout
              );

    void connect(Callback cb);

private:
    boost::asio::io_service&                  ioService_;
    const boost::asio::ip::tcp::endpoint      ep_       ;
    const std::chrono::steady_clock::duration timeout_  ;

    StrandPtr strand_;
};
} // namespace net

#endif // __W1WN67AALW42X198BRBOOP6LP2ZQFEK1GPPF4UDIDLDSHK71S7ABS911YK367EB5LL7YX3HOT__H__
