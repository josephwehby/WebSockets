#pragma once

#include "root_certificates.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <nlohmann/json.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "root_certificates.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;

class WebSocket : public std::enable_shared_from_this<WebSocket> {
  public:
    explicit WebSocket(net::io_context&, ssl::context&);
    void connect(std::string, std::string);
    void close();
  private:
    void onResolve(beast::error_code, tcp::resolver::results_type);
    void onConnect(beast::error_code, tcp::resolver::results_type::endpoint_type);
    void onHandshake(beast::error_code);
    void onSSLHandshake(beast::error_code);
    void onRead(beast::error_code, std::size_t);
    void doRead();
    void onWrite(beast::error_code, std::size_t);
    void send(json&);
    void onUnsubscribe(beast::error_code, std::size_t);
    void onClose(beast::error_code);
    
    std::string m_host = "ws.kraken.com";
    std::string m_port = "443";
    std::string m_symbol;
    std::string m_channel;

    tcp::resolver m_resolver;
    ssl::context m_ssl_ctx {ssl::context::tlsv12_client};
    websocket::stream<ssl::stream<beast::tcp_stream>> m_ws;
    beast::flat_buffer m_buffer;
};
