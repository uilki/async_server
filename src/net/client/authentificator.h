#ifndef __QU5JZH8FOO7ERQJ6ZXCHTJSY1L6GS2S9ME7R8HOVJ2M1EDTQSGY2ERPBH9M3DV53XG5CHAWT8__H__
#define __QU5JZH8FOO7ERQJ6ZXCHTJSY1L6GS2S9ME7R8HOVJ2M1EDTQSGY2ERPBH9M3DV53XG5CHAWT8__H__

#include "requestparser.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <future>

namespace client {
class Authentificator: public std::enable_shared_from_this<Authentificator>
{
public:
    Authentificator(boost::asio::ip::tcp::socket s,
                             std::promise<boost::asio::ip::tcp::socket> &authP,
                             std::string name,
                             std::string pass);
    ~Authentificator() = default;

    void start();
    void logIn();

private:
    bool writeAsync(std::string str);
    Authentificator(const Authentificator&) = delete;
    Authentificator& operator=(const Authentificator&) = delete;

    void onRead(const std::error_code &ec, std::uint32_t sz);
    void done(const std::error_code &ec);

    boost::asio::ip::tcp::socket s_;
    boost::asio::streambuf       b_;

    std::promise<boost::asio::ip::tcp::socket> &authP_;
    std::string         name_  ;
    std::string         pass_  ;
    util::RequestParser parser_;
};
} // namespace client

#endif // __QU5JZH8FOO7ERQJ6ZXCHTJSY1L6GS2S9ME7R8HOVJ2M1EDTQSGY2ERPBH9M3DV53XG5CHAWT8__H__
