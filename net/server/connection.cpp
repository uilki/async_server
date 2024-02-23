#include "connection.h"
#include "logger.h"

#include <boost/asio/write.hpp>
#include <boost/asio/read_until.hpp>

using tcp = boost::asio::ip::tcp;

namespace server {
Connection::Connection(boost::asio::ip::tcp::socket s, net::ThreadPool& p, RequestHandler& handler)
    : s_      {std::move(s)}
    , pool_   {p}
    , handler_{handler}
    { log_info << "Connected: " << s_.remote_endpoint();}

void Connection::start()
{
    boost::system::error_code ec;
    s_.set_option(tcp::no_delay(true), ec);
    if (ec)
        { log_info << "Connection: " << s_.remote_endpoint() << ".Cannot set TCP_NODELAY: " << ec.message(); }

    onRead({}, 0);
}

bool Connection::writeAsync(std::string str)
{
    auto message = std::make_shared<std::string>(std::move(str));
    boost::asio::async_write(s_,
                             boost::asio::buffer(*message),
                             [message](const boost::system::error_code& ec, std::size_t sz) {
                                 if (ec) {
                                     log_info << "onWrite(): " << ec.message()<< " " << ec.category().name()<< " " << ec.value();
                                     return;
                                 }

                                 log_info << "onWrite, " << sz << " bytes ";
                             });
    return true;
}

void Connection::onRead(const std::error_code &ec, uint32_t sz)
{
    if (ec) {
        log_info << "Close connection " << s_.remote_endpoint() << " with error code: " << ec.message();

        boost::system::error_code err;
        s_.close(err);

        return;
    }

    if (sz > 0) {
        std::istream is(&b_);
        std::string line;
        std::getline(is, line);

        pool_.post(std::bind([that=shared_from_this()](std::string request){ that->writeAsync(that->handler_(request)); },
                             std::move(line)));
    }

    boost::asio::async_read_until(s_,
                                  b_,
                                  "\n",
                                  std::bind(&Connection::onRead  ,
                                            shared_from_this()   ,
                                            std::placeholders::_1,
                                            std::placeholders::_2 ));
}
} // namespace server
