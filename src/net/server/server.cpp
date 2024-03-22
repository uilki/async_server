#include "server.h"
#include "strand.h"
#include "connection.h"
#include "logger.h"

#include <boost/asio/bind_executor.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>

using namespace boost::asio;

namespace net::server {
Server::Server(Executor &executor, const ip::tcp &protocol, RequestHandler handler)
    : io_service_ {executor.ioService()}
    , ssl_context_{ssl::context::sslv23}
    , strand_     {executor.strand()}
    , acceptor_   {io_service_, protocol}
    , pool_       {std::thread::hardware_concurrency()}
    , handler_    {std::move(handler)}
    {
    ssl_context_.set_options(ssl::context::default_workarounds |
                             ssl::context::no_sslv2            |
                             ssl::context::single_dh_use       |
                             ssl::context::verify_none          );
    ssl_context_.set_password_callback(std::bind(&Server::get_password, this));
    ssl_context_.use_certificate_chain_file("server.crt");
    ssl_context_.use_private_key_file("server.key", ssl::context::pem);
    ssl_context_.use_tmp_dh_file("dh2048.pem");
}
    std::string Server::get_password() const
    {
        return "test";
    }

    boost::system::error_code Server::start(const ip::tcp::endpoint &endpoint)
    {
    boost::system::error_code ec = bind(endpoint);
    acceptor_.set_option(ip::tcp::acceptor::reuse_address(true), ec);
    if (ec)
        { return ec; }

    ec = listen();
    if (ec)
        { return ec; }

    log_info <<  "Started at " << acceptor_.local_endpoint();

    return boost::system::error_code{};
}

boost::system::error_code Server::bind(const ip::tcp::endpoint &endpoint)
{
    boost::system::error_code ec;
    acceptor_.set_option(ip::tcp::acceptor::reuse_address(true), ec);
    if (ec)
        { return ec; }

    acceptor_.bind(endpoint, ec);
    if (ec)
        { return ec; }

    return boost::system::error_code{};
}

boost::system::error_code Server::listen()
{
    boost::system::error_code ec;
    acceptor_.set_option(ip::tcp::no_delay(true), ec);
    if (ec)
        { log_info << "Acceptor: cannot set TCP_NODELAY: " << ec.message(); }

    acceptor_.listen(backlog, ec);
    if (ec)
        { return ec; }

    startAccept();
    return boost::system::error_code{};
}

void Server::startAccept()
{
    acceptor_.async_accept(std::bind(&Server::onAccept      ,
                                       shared_from_this()   ,
                                       std::placeholders::_1,
                                       std::placeholders::_2 ));
}

void Server::stop()
{
    dispatch(*strand_, [sharedThis = shared_from_this()]() {
        assert(sharedThis->strand_->running_in_this_thread());

        boost::system::error_code ec;
        sharedThis->acceptor_.close(ec);
    });
}

void Server::onAccept(const boost::system::error_code &err, ip::tcp::socket s)
{
    if (err) {
        if (err != boost::system::error_code(error::operation_aborted))
            { log_info << "Acceptor: accept failed: " << err.message(); }
        else // acceptor stopped, don't accept again.
            { return; }
    }
    auto conn = std::make_shared<Connection>(ssl::stream<ip::tcp::socket>(std::move(s),
                                                                          ssl_context_),
                                             strand_                                   ,
                                             pool_                                     ,
                                             handler_                                   );
    conn->start();

    startAccept();
}
} // namespace namespace net::server
