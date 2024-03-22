#include "connector.h"
#include "logger.h"

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ssl.hpp>

using namespace boost::asio;
using Socket = ssl::stream<ip::tcp::socket>;

namespace net {
struct ConnectionOperation : public std::enable_shared_from_this<ConnectionOperation>
{
    ConnectionOperation(ServiceType &ioService, ssl::context& ctx,const StrandPtr& strand, Connector::Callback cb)
        : ioService_ {ioService}
        , timer_     {ioService_}
        , connection_{ioService_, ctx}
        , strand_    {strand}
        , cb_        {std::move(cb)}
        , cancelFlag_{false}
    {
        connection_.set_verify_mode(ssl::verify_peer);
        connection_.set_verify_callback(
            std::bind(&ConnectionOperation::verify_certificate, this,
                std::placeholders::_1, std::placeholders::_2));


        if (!SSL_set_tlsext_host_name(connection_.native_handle(), "localhost")) {
            boost::system::error_code ec{ static_cast<int>(::ERR_get_error()), error::get_ssl_category() };
            log_info << ec.message();
            return;
        }
    }

    void connect(ip::tcp::endpoint ep, std::chrono::steady_clock::duration timeout)
    {
        timer_.expires_from_now(timeout);
        timer_.async_wait(bind_executor(*strand_,
                          std::bind(&ConnectionOperation::onTimeout,
                                    shared_from_this()             ,
                                    cancelFlag_.load()              )));

        connection_.lowest_layer().async_connect(ep,
                                                 bind_executor(*strand_,
                                                 std::bind(&ConnectionOperation::onConnect,
                                                            shared_from_this()            ,
                                                            std::placeholders::_1          )));
    }

    void onTimeout(bool cancelled)
    {
        if (cancelFlag_ == cancelled) {
            assert(strand_->running_in_this_thread());

            boost::system::error_code ec;
            connection_.lowest_layer().close(ec);

            Connector::Callback cb;
            std::swap(cb, cb_);
            cb(std::move(connection_), boost::system::error_code(error::timed_out));
        }
    }

    void onConnect(boost::system::error_code ec)
    {
        assert(strand_->running_in_this_thread());

        timer_.cancel();
        cancelFlag_.store(true);

        if (ec){
            boost::system::error_code err;
            connection_.lowest_layer().close(err);
        }
        connection_.async_handshake(ssl::stream_base::client,
                                    std::bind(&ConnectionOperation::onHandshake, shared_from_this(),
                                              std::placeholders::_1));
    }

    void onHandshake(const boost::system::error_code& error)
    {
        if (error){
            boost::system::error_code err;
            connection_.lowest_layer().close(err);
        }

        Connector::Callback cb;
        std::swap(cb, cb_);
        cb(std::move(connection_), error);
    }
    bool verify_certificate(bool preverified, ssl::verify_context& ctx)
    {
        char subject_name[256];
        X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
        X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
        log_info << "Verifying " << subject_name << "\n";

        return preverified;
    }

    ServiceType&        ioService_ ;
    steady_timer        timer_     ;
    Socket              connection_;
    StrandPtr           strand_    ;
    Connector::Callback cb_        ;
    std::atomic_bool    cancelFlag_;
};

Connector::Connector(Executor &executor, ip::tcp::endpoint ep, std::chrono::steady_clock::duration timeout)
    : ioService_{executor.ioService()}
    , ep_       {ep}
    , timeout_  {timeout}
    , strand_   {executor.strand()}
    {}

void Connector::connect(ssl::context &ctx, Callback cb)
{
    auto connectionOperation = std::make_shared<ConnectionOperation>(ioService_, ctx, strand_, std::move(cb));
    connectionOperation->connect(ep_, timeout_);
}
} // namespace net
