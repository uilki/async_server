#include "connection.h"
#include "logger.h"

#include <boost/asio/write.hpp>
#include <boost/asio/read_until.hpp>

#include <future>

namespace  client {
Connection::Connection(boost::asio::ip::tcp::socket s)
    : s_            {std::move(s)}
    { log_info << "Connected: " << s_.remote_endpoint(); }

void Connection::start(DoneFn dfn)
{
    dfn_.swap(dfn);
    onRead({}, 0);
}

bool Connection::writeAsync(std::string str)
{
    std::promise<bool> p;
    auto f = p.get_future();

    auto firstSpace = str.find_first_of(' ');

    // We want this to be executed on ioService, i.e. serialized with read code.
    boost::asio::async_write(s_,
                             boost::asio::buffer(parser_.fromString(transactionId_++, str.substr(0, firstSpace), str.substr(firstSpace + 1))),
                             [this, &p](const boost::system::error_code& ec, std::size_t sz) {
                                 if (!dfn_) {
                                     p.set_value(false);
                                     return;
                                 }

                                 if (ec) {
                                     log_info << "onWrite(): " << ec.message();
                                     done(ec);
                                     p.set_value(false);
                                     return;
                                 }

                                 log_info << "onWrite(" << sz << ")";

                                 p.set_value(true);
                            });

    return f.get();
}

void Connection::onRead(const std::error_code &ec, uint32_t sz)
{
    if (!dfn_)
    { return; }

    if (ec) {
        log_info << "onRead(): " << ec.message();
        done(ec);
        return;
    }

    if (sz > 0) {
        std::istream is(&b_);
        std::string line;
        std::getline(is, line);

        try
        {
            util::RequestParser::Request parsedResponse = parser_.parse(line.c_str());
            std::ostringstream os;
            os << "\nServer responce:\nTransaction id: " << parsedResponse.id << '\n';
            for (const auto& v : parsedResponse.args)
                { os << '\t' << v << '\n' ; }
            log_info << os.str();
        }
        catch (util::BadRequest& e)
            { log_info << "Failed to parse response: " << e.what(); }
    }

    boost::asio::async_read_until(s_,
                                  b_,
                                  "\n",
                                  std::bind(&Connection::onRead  ,
                                            shared_from_this()   ,
                                            std::placeholders::_1,
                                            std::placeholders::_2 ));
}

void Connection::done(const std::error_code& ec)
{
    boost::system::error_code err;
    s_.close(err);

    DoneFn cb;
    std::swap(dfn_, cb);
    cb(ec);
}

Connection::TransactionID Connection::TransactionID::operator ++(int)
{
    auto temp = *this;
    if (id_ == std::numeric_limits<uint16_t>::max()) id_ = util::RequestParser::Request::invalidID + 1;
    else ++id_;
    return temp;
}

} // namespace  client
