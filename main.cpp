#include "root_certificates.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

#include "WebSocket.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;

int main(int argc, char** argv)
{
    net::io_context ioc;
    boost::system::error_code ec;
    ssl::context ctx(ssl::context::tlsv12_client);


    load_root_certificates(ctx, ec);

    if (ec) {
      std::cout << "Error" << ec << std::endl;
      return -1;
    }
    
    auto client = std::make_shared<WebSocket>(ioc, ctx);
    client->connect("ticker", "BTC/USD");

    std::cout << "[!] Open Websocket for ticker BTC/USD" << std::endl;
    std::thread io_thread([&ioc]() {
        ioc.run();
    });

    std::cout << "Press Enter to stop the WebSocket connection..." << std::endl;
    std::cin.get();  // Waits for the user to press Enter

    std::cout << "Closing WebSocket..." << std::endl;

    // Join the IO thread before exiting
    if (io_thread.joinable()) {
        io_thread.join();
    }

    client->close();

    return 0;
}
