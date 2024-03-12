#include "connection.h"
#include "logger.h"

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read_until.hpp>

using tcp = boost::asio::ip::tcp;

namespace net::server {
Connection::Connection(boost::asio::ip::tcp::socket s, net::StrandPtr strand, net::ThreadPool& p, RequestHandler& handler)
    : s_      {std::move(s)}
    , strand_ {strand}
    , timer_  {s_.get_executor()}
    , timeout_{std::chrono::seconds{300}}
    , pool_   {p}
    , handler_{handler}
    , signed_ {false}
    { log_info << "Connected: " << s_.remote_endpoint();}

void Connection::start()
{
    boost::system::error_code ec;
    s_.set_option(tcp::no_delay(true), ec);
    if (ec)
        { log_info << "Connection: " << s_.remote_endpoint() << ".Cannot set TCP_NODELAY: " << ec.message(); }

    timer_.expires_from_now(timeout_);
    timer_.async_wait(boost::asio::bind_executor(*strand_,std::bind(&Connection::onTimeout,shared_from_this(), std::placeholders::_1)));
    onRead({}, 0);
}

bool Connection::writeAsync(std::string str)
{
    auto message = std::make_shared<std::string>(std::move(str));
    boost::asio::async_write(s_,
                             boost::asio::buffer(*message),
                             [message](const boost::system::error_code& ec, std::size_t sz) {
                                 if (ec) {
                                     log_info << "Connection onWrite(): " << ec.message()<< " " << ec.category().name()<< " " << ec.value();
                                     return;
                                 }

                                 log_info << "Connection onWrite, " << *message;
                             });
    return true;
}

void Connection::onRead(const std::error_code &ec, uint32_t sz)
{
    if (ec)
        { return closeConnection(ec.message()); }

    if (sz > 0) {
        std::istream is(&b_);
        std::string line;
        std::getline(is, line);

        pool_.post(std::bind([that=shared_from_this()](std::string request)
                             {
                                 that->writeAsync(that->handler_(request, that->signed_));
                                 if (that->signed_)
                                     { that->timer_.cancel();}
                             },
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

void Connection::closeConnection(const std::string& mes)
{
    log_info << "Close connection " << s_.remote_endpoint() << " with error code: " << mes;

    boost::system::error_code err;
    s_.close(err);

    return;
}

void Connection::onTimeout(const std::error_code &ec)
{
    if (!ec)
        { closeConnection(std::to_string(timeout_.count()) +" sec without signing"); }
}
} // namespace net::server
