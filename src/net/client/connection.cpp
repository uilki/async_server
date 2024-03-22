#include "connection.h"
#include "request.h"
#include "logger.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read_until.hpp>

#include <future>

using Request   = common::SessionRequest;
using StringList = std::list<std::string>;

using namespace boost::asio;

namespace  client {
Connection::Connection(Socket s)
    : s_            {std::move(s)}
    , parser_       {"requestSchema.json"}
    , transactionId_{common::SessionRequest::BadRequest + 1}
    { log_info << "Connected: " << s_.lowest_layer().remote_endpoint(); }

Connection::~Connection()
    { closeSocket(); }

void Connection::start(DoneFn dfn)
{
    dfn_.swap(dfn);
    onRead({}, 0);
}

bool Connection::writeAsync(std::string str)
{
    auto [funName, args] = [](std::string str) {
        boost::trim(str);
        StringList result;
        boost::split(result, std::move(str), boost::is_space(), boost::token_compress_on);
        auto funName = result.front();
        result.pop_front();
        return std::make_pair(funName, result);
    }(std::move(str));

    auto mesPtr = std::make_shared<std::string>(parser_.toJson(getTransactionId(), funName, args) + '\n');

    async_write(s_,
                buffer(*mesPtr),
                [this, mesPtr](const boost::system::error_code& ec, std::size_t sz) {
                    if (!dfn_)
                        { return; }

                    if (ec) {
                        log_info << "Connection onWrite(): " << ec.message();
                        done(ec);
                        return;
                    }

                    log_info << "Connection onWrite(" << *mesPtr << ")";
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
         { processResponse(parser_.parse<Request,size_t, std::string, StringList>(line.c_str())); }
        catch (util::BadRequest& e)
            { log_info << "Failed to parse response: " << e.what(); }
    }

    async_read_until(s_,
                     b_,
                     "\n",
                     std::bind(&Connection::onRead  ,
                               shared_from_this()   ,
                               std::placeholders::_1,
                               std::placeholders::_2 ));
}

void Connection::closeSocket()
{
    boost::system::error_code err;
    s_.shutdown();
    s_.lowest_layer().close(err);
}

void Connection::done(const std::error_code& ec)
{
    closeSocket();

    DoneFn cb;
    std::swap(dfn_, cb);
    cb(ec);
}

void Connection::processResponse(const Request &r) const
{
    std::ostringstream os;
    os << "\nServer responce:\nTransaction id: " << r.id << '\n';
    for (const auto& v : r.args)
        { os << '\t' << v << '\n' ; }
    log_info << os.str();
}

uint64_t Connection::getTransactionId()
{
    auto temp = transactionId_;
    if (transactionId_ == std::numeric_limits<uint16_t>::max()) transactionId_ = Request::BadRequest + 1;
    else ++transactionId_;
    return temp;
}
} // namespace  client
