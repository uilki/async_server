#include "connector.h"

#include <boost/asio/bind_executor.hpp>

using tcp = boost::asio::ip::tcp;

namespace net {
struct ConnectionOperation : public std::enable_shared_from_this<ConnectionOperation>
{
    ConnectionOperation(ServiceType &ioService, const StrandPtr& strand, Connector::Callback cb)
        : ioService_ {ioService}
        , timer_     {ioService_}
        , connection_{ioService_}
        , strand_    {strand}
        , cb_        {std::move(cb)}
        , cancelFlag_{false}
    {}

    void connect(tcp::endpoint ep, std::chrono::steady_clock::duration timeout)
    {
        timer_.expires_from_now(timeout);
        timer_.async_wait(boost::asio::bind_executor(*strand_,
                                                     std::bind(&ConnectionOperation::onTimeout,
                                                               shared_from_this()             ,
                                                               cancelFlag_.load()              )));
        connection_.async_connect(ep,
                                  boost::asio::bind_executor(*strand_,
                                                             std::bind(&ConnectionOperation::onConnect,
                                                                       shared_from_this()             ,
                                                                       std::placeholders::_1           )));
    }

    void onTimeout(bool cancelled)
    {
        if (cancelFlag_ == cancelled) {
            assert(strand_->running_in_this_thread());

            boost::system::error_code ec;
            connection_.close(ec);

            Connector::Callback cb;
            std::swap(cb, cb_);
            cb(std::move(connection_), boost::system::error_code(boost::asio::error::timed_out));
        }
    }

    void onConnect(boost::system::error_code ec)
    {
        assert(strand_->running_in_this_thread());

        timer_.cancel();
        cancelFlag_.store(true);

        if (ec){
            boost::system::error_code err;
            connection_.close(err);
        }

        Connector::Callback cb;
        std::swap(cb, cb_);
        cb(std::move(connection_), ec);
    }

    ServiceType&              ioService_ ;
    boost::asio::steady_timer timer_     ;
    tcp::socket               connection_;
    StrandPtr                 strand_    ;
    Connector::Callback       cb_        ;
    std::atomic_bool          cancelFlag_;
};

Connector::Connector(Executor &executor, tcp::endpoint ep, std::chrono::steady_clock::duration timeout)
    : ioService_{executor.ioService()}
    , ep_       {ep}
    , timeout_  {timeout}
    , strand_   {executor.strand()}
    {}

void Connector::connect(Callback cb)
{
    auto connectionOperation = std::make_shared<ConnectionOperation>(ioService_, strand_, std::move(cb));
    connectionOperation->connect(ep_, timeout_);
}
} // namespace net
