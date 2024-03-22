#include "authentificator.h"
#include "request.h"
#include "logger.h"

#include <boost/asio/write.hpp>
#include <boost/asio/read_until.hpp>

#include <future>

namespace  client {
const char* someToken = "GDHEU26467";

using Request = common::AuthentificationRequest;
using StringList = std::list<std::string>;

using namespace boost::asio;
Authentificator::Authentificator(ssl::stream<ip::tcp::socket> s                   ,
                                 std::promise<ssl::stream<ip::tcp::socket>> &authP,
                                 std::string name                                 ,
                                 std::string pass                                  )
    : s_     {std::move(s)}
    , authP_ {authP}
    , name_  {std::move(name)}
    , pass_  {std::move(pass)}
    , parser_{"authRequestSchema.json"}
    {}

void Authentificator::start()
    { onRead({}, 0); }

void Authentificator::logIn()
    { writeAsync(parser_.toJson(Request::signIn, StringList{name_, pass_}) + '\n'); }

bool Authentificator::writeAsync(std::string str)
{
    auto mes = std::make_shared<std::string>(str);
    async_write(s_,
                buffer(str),
                [that = shared_from_this(), mes](const boost::system::error_code& ec, std::size_t sz) {
                    if (ec) {
                        log_info << "Authentificator onWrite(): " << ec.message();
                        that->done(ec);
                        return;
                    }

                    log_info << "Authentificator onWrite(" << *mes << ")";
                 });
    return true;
}

void Authentificator::onRead(const std::error_code &ec, uint32_t sz)
{
    if (ec) {
        log_info << "onRead(): " << ec.message();
        done(ec);
        return;
    }

    if (sz > 0) {
        std::istream is(&b_);
        std::string line;
        std::getline(is, line);
        log_info <<line;
        try
        {
            Request parsedResponse = parser_.parse<Request, size_t>(line.c_str());
            switch (parsedResponse.id) {
                case Request::unregistered: post(s_.get_executor(),
                                                 std::bind(&Authentificator::writeAsync,
                                                           shared_from_this(),
                                                           parser_.toJson(Request::signUp,
                                                                          StringList{someToken,
                                                                                     name_    ,
                                                                                     pass_     }) + '\n'));
                    break;
                case Request::success     : authP_.set_value(std::move(s_)); return;
                default                   : done({}) ; return;
            }
        }
        catch (util::BadRequest& e)
            { log_info << "Failed to parse response: " << e.what(); }
    }

    async_read_until(s_,
                     b_,
                     "\n",
                     std::bind(&Authentificator::onRead  ,
                               shared_from_this()   ,
                               std::placeholders::_1,
                               std::placeholders::_2 ));
}

void Authentificator::done(const std::error_code& ec)
{
    boost::system::error_code err;
    s_.lowest_layer().close(err);
    authP_.set_value(std::move(s_));
}
} // namespace  client
