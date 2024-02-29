#include "connection.h"
#include "logger.h"

#include <boost/asio/write.hpp>
#include <boost/asio/read_until.hpp>

#include <future>

namespace  client {
Connection::Connection(boost::asio::ip::tcp::socket s)
    : s_     {std::move(s)}
    { log_info << "Connected: " << s_.remote_endpoint(); }

void Connection::start(DoneFn dfn)
{
    dfn_.swap(dfn);
    onRead({}, 0);
}

bool Connection::writeAsync(std::string str)
{
    auto firstSpace = str.find_first_of(' ');
    auto mes = std::make_shared<std::string>(parser_.fromString(transactionId_++, str.substr(0, firstSpace), str.substr(firstSpace + 1)));

    boost::asio::async_write(s_,
                             boost::asio::buffer(*mes),
                             [this, mes](const boost::system::error_code& ec, std::size_t sz) {
                                 if (!dfn_)
                                     { return; }

                                 if (ec) {
                                     log_info << "Connection onWrite(): " << ec.message();
                                     done(ec);
                                     return;
                                 }

                                 log_info << "Connection onWrite(" << *mes << ")";
                             });

    return true;
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
            { processResponse(parser_.parse(line.c_str())); }
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

void Connection::processResponse(const util::RequestParser::Request &r) const
{
    std::ostringstream os;
    os << "\nServer responce:\nTransaction id: " << r.id << '\n';
    for (const auto& v : r.args)
        { os << '\t' << v << '\n' ; }
    log_info << os.str();
}

Connection::TransactionID Connection::TransactionID::operator ++(int)
{
    auto temp = *this;
    if (id_ == std::numeric_limits<uint16_t>::max()) id_ = util::RequestParser::Request::BadRequest + 1;
    else ++id_;
    return temp;
}
} // namespace  client
