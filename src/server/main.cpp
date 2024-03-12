#include "executor.h"
#include "logger.h"
#include "server.h"
#include "requestparser.h"
#include "request.h"
#include "registry.h"
#include "auth/authentificator.h"
#include "auth/serverfabric.h"

#include <iostream>
#include <memory>
#include <future>
#include <functional>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>


const char* someToken = "GDHEU26467";

using StringList = std::list<std::string>;

int main()
{
    std::uint16_t ipPort = 8081;

    net::server::Authentificator a{net::server::auth::PasswordServer::getPasswordServer(net::server::AbstractPasswordServer::Kind::file, "srvpass")};
    std::hash<std::string> hasher;

    net::Executor executor{};

    auto requestHandler = [requestParser = util::RequestParser{"requestSchema.json"}] (std::string request) mutable
    {
        using Request = common::SessionRequest;
        log_info << request;
        Request parsedRequest{};
        try
            { parsedRequest = requestParser.parse<Request, size_t, std::string, StringList>(request.c_str()); }
        catch (util::BadRequest& e)
            { return requestParser.toJson(Request::BadRequest, "Parse error") + '\n'; }


        auto toList = [](std::string str) {
            boost::trim(str);
            StringList result;
            boost::split(result, str, boost::is_space(), boost::token_compress_on);
            return result;
        };

        StringList result;
        try
            { result.splice(result.cend(), toList(holder::Registry::registry().call(parsedRequest.name, parsedRequest.args))); }
        catch (std::exception& e)
            { result.push_back(e.what()); }
        return requestParser.toJson(parsedRequest.id, parsedRequest.name, result) + '\n';

    };

    auto autehtificationHandler = [parser = util::RequestParser{"authRequestSchema.json"}, &a, &hasher](std::string request, bool& isSigned) mutable
    {
        using Request = common::AuthentificationRequest;
        Request parsedRequest{};
        try
            { parsedRequest = parser.parse<Request, size_t, StringList>(request.c_str()); }
        catch (util::BadRequest& e)
            { return parser.toJson(Request::ErrorCode::BadRequest) + '\n'; }
        switch (parsedRequest.id) {
        case Request::signUp: {
            auto it = parsedRequest.data.begin();
            if (*it != someToken)
                { return parser.toJson(Request::invalidToken) + '\n'; }
            auto name = *++it;
            if (!a.signUp(name, hasher(*++it)))
                { return parser.toJson(Request::userExists) + '\n'; }
            isSigned = true;
            return parser.toJson(Request::success) + '\n';
        }
        case Request::signIn:{
            using Result = net::server::Authentificator::Result;
            auto name = *parsedRequest.data.begin();
            switch (a.sighIn(name, hasher(*++parsedRequest.data.begin()))) {
            case Result::signIn       : isSigned = true; return parser.toJson(Request::success     ) + '\n';
            case Result::unregistered :                  return parser.toJson(Request::unregistered) + '\n';
            case Result::wrongpassword:                  return parser.toJson(Request::badPassword ) + '\n';
            default                   :                  return parser.toJson(Request::BadRequest  ) + '\n';
            }
        }
        default:
            return parser.toJson(Request::BadRequest) + '\n';
        }
    };

    auto handler = [&requestHandler, &autehtificationHandler](std::string request, bool& isSigned) mutable -> std::string
        { return isSigned ? requestHandler(request) : autehtificationHandler(request, isSigned); };

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
