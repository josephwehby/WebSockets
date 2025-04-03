#include <thread>
#include <chrono>
#include "WebSocket.hpp"

int main() {
    net::io_context ioc;
    boost::system::error_code ec;
    ssl::context ctx(ssl::context::tlsv12_client);

    load_root_certificates(ctx, ec);

    if (ec) {
      std::cout << "Error" << ec << std::endl;
      return -1;
    }
    
    auto client = std::make_shared<WebSocket>(ioc, ctx);

    std::cout << "[!] Open Websocket for ticker BTC/USD" << std::endl;
    std::thread io_thread([&ioc]() {
        ioc.run();
    });

    client->connect();
//    std::this_thread::sleep_for(std::chrono::seconds(2));    
    client->subscribe("XMR/USD", "ticker");

    std::cout << "Press Enter to stop the WebSocket connection..." << std::endl;
    std::cin.get();
    
    client->unsubscribe("XMR/USD", "ticker");
    std::cout << "Closing WebSocket..." << std::endl;
    client->close();

    ioc.stop();

    // Join the IO thread before exiting
    if (io_thread.joinable()) {
        io_thread.join();
    }


    return 0;
}
