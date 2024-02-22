#ifndef __HUABF6FV4DJB7G20UMOIGCWD3W6BUG99T8EPYC49BXVO34YHHJ9DNA1SQ0VSTUHXLRAK3TJ2I__H__
#define __HUABF6FV4DJB7G20UMOIGCWD3W6BUG99T8EPYC49BXVO34YHHJ9DNA1SQ0VSTUHXLRAK3TJ2I__H__

#include "request.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>

namespace client {
class Connection: public std::enable_shared_from_this<Connection>
{
    struct TransactionID {
        TransactionID operator++(int);
        inline operator uint16_t() const { return id_; };
        uint16_t id_{util::RequestParser::Request::invalidID + 1};
    };

    using DoneFn = std::function<void(const std::error_code&)>;
public:
    using ConnectionPtr = std::shared_ptr<Connection>;
    explicit Connection(boost::asio::ip::tcp::socket s);
    ~Connection() = default;

    void start(DoneFn dfn);
    bool writeAsync(std::string str);

private:
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    void onRead(const std::error_code &ec, std::uint32_t sz);

    void done(const std::error_code &ec);

    boost::asio::ip::tcp::socket s_;
    boost::asio::streambuf       b_;

    DoneFn              dfn_          ;
    util::RequestParser parser_       ;
    TransactionID       transactionId_;
};
} // namespace client

#endif //__HUABF6FV4DJB7G20UMOIGCWD3W6BUG99T8EPYC49BXVO34YHHJ9DNA1SQ0VSTUHXLRAK3TJ2I__H__
