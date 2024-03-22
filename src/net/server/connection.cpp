#include "connection.h"
#include "logger.h"

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read_until.hpp>

using namespace boost::asio;

namespace net::server {
Connection::Connection(Socket s, StrandPtr strand, ThreadPool& p, RequestHandler& handler)
    : s_      {std::move(s)}
    , timer_  {s_.lowest_layer().get_executor()}
    , strand_ {strand}
    , pool_   {p}
    , handler_{handler}
    , signed_ {false}
    , timeout_{std::chrono::seconds{300}}
    { log_info << "Connected: " << s_.lowest_layer().remote_endpoint(); }

Connection::~Connection()
    { closeConnection(""); }

void Connection::start()
{
    boost::system::error_code ec;
    s_.lowest_layer().set_option(ip::tcp::no_delay(true), ec);
    if (ec)
    { log_info << "Connection: " << s_.lowest_layer().remote_endpoint() << ".Cannot set TCP_NODELAY: " << ec.message(); }
    s_.async_handshake(ssl::stream_base::server,
                       std::bind(&Connection::handleHandshake,
                                 shared_from_this(),
                                 std::placeholders::_1));
}

void Connection::handleHandshake(const boost::system::error_code& error)
{
    if (error)
        { closeConnection(error.message()); }

    timer_.expires_from_now(timeout_);
    timer_.async_wait(bind_executor(*strand_, std::bind(&Connection::onTimeout,shared_from_this(), std::placeholders::_1)));
    onRead({}, 0);
}

bool Connection::writeAsync(std::string str)
{
    auto mesPtr = std::make_shared<std::string>(std::move(str));
    async_write(s_,
                buffer(*mesPtr),
                [mesPtr](const boost::system::error_code& ec, std::size_t sz) {
                    if (ec) {
                        log_info << "Connection onWrite(): " << ec.message()<< " " << ec.category().name()<< " " << ec.value();
                        return;
                    }
                    log_info << "Connection onWrite, " << *mesPtr;
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

    async_read_until(s_,
                     b_,
                     "\n",
                     std::bind(&Connection::onRead  ,
                               shared_from_this()   ,
                               std::placeholders::_1,
                               std::placeholders::_2 ));
}

void Connection::closeConnection(const std::string& mes)
{
    if (s_.lowest_layer().is_open()) {
        log_info << "Close connection " << s_.lowest_layer().remote_endpoint();

        boost::system::error_code err;
        s_.shutdown();
        s_.lowest_layer().close(err);
    }
}

void Connection::onTimeout(const std::error_code &ec)
{
    if (!ec)
        { closeConnection("Timeout"); }
}
} // namespace net::server
