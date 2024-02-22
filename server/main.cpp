#include "executor.h"
#include "server.h"
#include "request.h"
#include "registry.h"

#include <iostream>
#include <memory>
#include <future>

int main()
{
    std::uint16_t ipPort = 8081;
    net::Executor executor{};
    util::RequestParser parser{};
    auto handler = [&parser](std::string request) -> std::string
    {
        util::RequestParser::Request parsedRequest{};
        try
            { parsedRequest = parser.parse(request.c_str()); }
        catch (util::BadRequest& e)
            { return parser.fromString(util::RequestParser::Request::invalidID, util::RequestParser::Request::invalidMessage); }

        std::string result;
        try
            { result += holder::Registry::registry().call(parsedRequest.name, parsedRequest.args); }
        catch (std::exception& e)
            { result += e.what(); }

        return parser.fromString(parsedRequest.id, parsedRequest.name, result);
    };

    auto server = std::make_shared<net::Server>(executor, boost::asio::ip::tcp::v4(), std::move(handler));

    auto ec = server->start(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), ipPort));
    if (ec) {
        std::cerr << "Can't start server: " << ec.message() << '\n';
        return 1;
    }

    std::promise<void> p;
    auto f = p.get_future();

    static std::promise<void>* gP = &p;
    ::signal(SIGINT, [](int) {
        if (gP) {
            gP->set_value();
            gP = nullptr;
        }
    });

    f.get();

    std::cout <<  "Shutting down..." << '\n';
    executor.stop();
    std::cout << "Stopped" << '\n';

    return 0;
}
