#include "executor.h"
#include "logger.h"
#include "server.h"
#include "request.h"
#include "registry.h"
#include "auth/authentificator.h"
#include "auth/serverfabric.h"

#include <iostream>
#include <memory>
#include <future>
#include <functional>

const char* someToken = "GDHEU26467";
int main()
{
    std::uint16_t ipPort = 8081;
    util::RequestParser parser{};
    net::server::Authentificator a{net::server::auth::PasswordServer::getPasswordServer(net::server::AbstractPasswordServer::Kind::file, "srvpass")};
    std::hash<std::string> hasher;

    net::Executor executor{};

    auto handler = [&parser, &a, &hasher](std::string request, bool& isSigned) -> std::string
    {
        using Request = util::RequestParser::Request;
        log_info << request;
        util::RequestParser::Request parsedRequest{};
        try
            { parsedRequest = parser.parse(request.c_str()); }
        catch (util::BadRequest& e)
            { return parser.fromString(Request::BadRequest, Request::invalidMessage); }


        if (!isSigned){
            switch (parsedRequest.id) {
            case Request::signUp: {
                if (parsedRequest.name != someToken) {
                    return parser.fromString(Request::invalidToken);
                }
                auto name = parsedRequest.args.front();
                parsedRequest.args.pop_front();
                auto hash = hasher(parsedRequest.args.front());
                if (!a.signUp(name, hash)) {
                    return parser.fromString(Request::userExists);
                }
                isSigned = true;
                return parser.fromString(Request::success);
            }
            case Request::signIn:{
                using Result = net::server::Authentificator::Result;
                switch (a.sighIn(parsedRequest.name, hasher(parsedRequest.args.front()))) {
                case Result::signIn       : isSigned = true; return parser.fromString(Request::success);
                case Result::unregistered : return parser.fromString(Request::unregistered);
                case Result::wrongpassword: return parser.fromString(Request::badPassword);
                default                   : return parser.fromString(Request::BadRequest, "SignIn required");
                }
            }
            default:
                return parser.fromString(util::RequestParser::Request::BadRequest, "SignIn required");
            }
        }

        std::string result;
        try
            { result += holder::Registry::registry().call(parsedRequest.name, parsedRequest.args); }
        catch (std::exception& e)
            { result += e.what(); }

        return parser.fromString(parsedRequest.id, parsedRequest.name, result);
    };

    auto server = std::make_shared<net::server::Server>(executor, boost::asio::ip::tcp::v4(), std::move(handler));

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
