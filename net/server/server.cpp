#include "server.h"
#include "auth/serverfabric.h"
#include "strand.h"
#include "connection.h"
#include "logger.h"

#include <boost/asio/bind_executor.hpp>

using tcp = boost::asio::ip::tcp;

namespace net::server {
Server::Server(Executor &executor, const tcp &protocol, RequestHandler handler)
    : io_service_{executor.ioService()}
    , strand_    {executor.strand()}
    , acceptor_  {io_service_, protocol}
    , pool_      {std::thread::hardware_concurrency()}
    , handler_   {std::move(handler)}
    {}

boost::system::error_code Server::start(const tcp::endpoint &endpoint)
{
    boost::system::error_code ec = bind(endpoint);
    acceptor_.set_option(tcp::acceptor::reuse_address(true), ec);
    if (ec)
        { return ec; }

    ec = listen();
    if (ec)
        { return ec; }

    log_info <<  "Started at " << acceptor_.local_endpoint();

    return boost::system::error_code{};
}

boost::system::error_code Server::bind(const tcp::endpoint &endpoint)
{
    boost::system::error_code ec;
    acceptor_.set_option(tcp::acceptor::reuse_address(true), ec);
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
    acceptor_.set_option(tcp::no_delay(true), ec);
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
    acceptor_.async_accept(boost::asio::bind_executor(*strand_,
                                                      std::bind(&Server::onAccept,
                                                                shared_from_this(),
                                                                std::placeholders::_1,
                                                                std::placeholders::_2 )));
}

void Server::stop()
{
    boost::asio::dispatch(*strand_, [sharedThis = shared_from_this()]() {
        assert(sharedThis->strand_->running_in_this_thread());

        boost::system::error_code ec;
        sharedThis->acceptor_.close(ec);
    });
}

void Server::onAccept(const std::error_code &err, tcp::socket socket)
{
    if (err) {
        if (err != boost::system::error_code(boost::asio::error::operation_aborted))
            { log_info << "Acceptor: accept failed: " << err.message(); }
        else // acceptor stopped, don't accept again.
            { return; }
    }
    auto conn = std::make_shared<server::Connection>(std::move(socket), strand_, pool_, handler_);
    conn->start();

    startAccept();
}
} // namespace namespace net::server
