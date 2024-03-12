#ifndef __HUABF6FV4DJB7G20UMOIGCWD3W6BUG99T8EPYC49BXVO34YHHJ9DNA1SQ0VSTUHXLRAK3TJ2I__H__
#define __HUABF6FV4DJB7G20UMOIGCWD3W6BUG99T8EPYC49BXVO34YHHJ9DNA1SQ0VSTUHXLRAK3TJ2I__H__

#include "requestparser.h"
#include "../../common/request.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>

namespace client {
class Connection: public std::enable_shared_from_this<Connection>
{
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
    void processResponse(const common::SessionRequest& r) const;
    uint64_t getTransactionId();

    boost::asio::ip::tcp::socket s_;
    boost::asio::streambuf       b_;

    DoneFn              dfn_          ;
    util::RequestParser parser_       ;
    uint64_t            transactionId_;
};
} // namespace client

#endif //__HUABF6FV4DJB7G20UMOIGCWD3W6BUG99T8EPYC49BXVO34YHHJ9DNA1SQ0VSTUHXLRAK3TJ2I__H__
