
#include "executor.h"
#include "connector.h"
#include "connection.h"
#include "authentificator.h"
#include <future>
#include <iostream>

#include <boost/program_options.hpp>

int main(int argc, char* argv[])
{
    boost::program_options::variables_map vm;
    std::string   ipAddressStr = "127.0.0.1";
    std::uint16_t ipPort = 8081;
    std::uint32_t connectTimeoutMs = 5000;

    try {
        boost::program_options::options_description desc("Options");

        desc.add_options()
            ("help", "Print this help message")
            ("ip-address", boost::program_options::value(&ipAddressStr), "IP address (numeric), default = 127.0.0.1")
            ("port", boost::program_options::value(&ipPort), "IP port (numeric), default = 8081")
            ("connect-timeout-ms", boost::program_options::value(&connectTimeoutMs), "Connect timeout (ms), default = 5000");

        boost::program_options::store(
            boost::program_options::command_line_parser(
                argc, argv).options(desc).run(), vm);

        boost::program_options::notify(vm);

        if (vm.count("help") > 0) {
            std::cout << desc << std::endl;
            return 1;
        }
    } catch (const boost::program_options::error& e) {
        std::cerr << "Invalid command line arguments: " << e.what() << '\n';
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    boost::system::error_code ec;
    auto ipAddress = boost::asio::ip::address::from_string(ipAddressStr, ec);
    if (ec) {
        std::cerr << "Cannot parse IP address: " << ec.message() << '\n';
        return 1;
    }

    net::Executor executor;

    net::Connector connector(executor,
        boost::asio::ip::tcp::endpoint(ipAddress, ipPort), std::chrono::milliseconds(connectTimeoutMs));

    std::cout << "Start client" << std::endl;

    std::promise<client::Connection::ConnectionPtr> connP;
    std::promise<void> doneP;
    static std::promise<void>* pDonePromise = &doneP;

    auto connF = connP.get_future();
    auto doneF = doneP.get_future();

    ::signal(SIGINT, [](int) {
        if (pDonePromise) {
            pDonePromise->set_value();
            pDonePromise = nullptr;
        }
    });

    std::promise<std::shared_ptr<client::Authentificator>> authP;
    auto authF = authP.get_future();
    std::promise<boost::asio::ip::tcp::socket> sockP;
    auto sockF = sockP.get_future();

    connector.connect([&authP, &sockP](boost::asio::ip::tcp::socket s, const std::error_code& ec) {
        if (ec) {
            std::cerr << "Failed to connect: " << ec.message() << '\n';
            authP.set_value({});
            return;
        }

        auto auth= std::make_shared<client::Authentificator>(std::move(s),sockP,"name", "password");
        auth->start();

        authP.set_value(auth);
    });

    auto auth = authF.get();

    if (!auth) {
        std::cerr << "Failed to connect: " << 2 << '\n';
        return 11;
    }
    auth->logIn();
    auto sock = sockF.get();
    if (!sock.is_open()) return 5;
    auto conn = std::make_shared<client::Connection>(std::move(sock));
    conn->start([&doneP](const std::error_code& ec) {
        std::cout << "Connection done: " << ec.message() << '\n';
        doneP.set_value();
    });
    //connP.set_value(conn);

    if (conn) {
        std::cout << "Type \"exit\" to disconnect\n";

        std::string line;
        while (std::getline(std::cin, line)) {

            if (line == "exit")
                { break; }
            line += '\n';
            if (!conn->writeAsync(line))
                { break; }
        }
        if (!std::cin) // Even if stdin is done we still want to listen for server data...
            { doneF.get(); }
    }

    std::cout << "Shutting down..." << '\n';
    executor.stop();
    std::cout << "Client stopped" << '\n';

    return 0;
}
