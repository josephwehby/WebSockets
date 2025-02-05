#include "WebSocket.hpp"

void fail(beast::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

WebSocket::WebSocket(net::io_context& ioc, ssl::context& ctx) 
  : m_resolver(net::make_strand(ioc)),
    m_ws(net::make_strand(ioc), m_ssl_ctx) {

      m_ssl_ctx = std::move(ctx);
}

void WebSocket::connect(std::string channel, std::string symbol) {
  m_symbol = symbol;
  m_channel = channel;
  m_resolver.async_resolve(m_host, m_port, beast::bind_front_handler(&WebSocket::onResolve, shared_from_this()));
}

void WebSocket::close() {
  if (!m_ws.is_open()) return;
  
  json unsubscribe_msg = {
    {"method", "unsubscribe"},
    {"params", {
      {"channel", m_channel},
      {"symbol", {m_symbol}}
    }}
  };
  
  m_ws.async_write(net::buffer(unsubscribe_msg.dump()), 
      beast::bind_front_handler(&WebSocket::onUnsubscribe, shared_from_this())
      ); 
}

void WebSocket::onResolve(beast::error_code ec, tcp::resolver::results_type results) {
  if (ec) {
    return fail(ec, "resolve");
  }

  beast::get_lowest_layer(m_ws).expires_after(std::chrono::seconds(30));

  beast::get_lowest_layer(m_ws).async_connect(
      results,
      beast::bind_front_handler(
        &WebSocket::onConnect, shared_from_this())
      );
}

void WebSocket::onConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep) {
  if (ec) {
    return fail(ec, "connect");
  }

  beast::get_lowest_layer(m_ws).expires_after(std::chrono::seconds(30));

  if (!SSL_set_tlsext_host_name(m_ws.next_layer().native_handle(), m_host.c_str())) {
    ec = beast::error_code(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category());
    return fail(ec, "connect");
  }

  m_host += ':' + std::to_string(ep.port());

  m_ws.next_layer().async_handshake(
      ssl::stream_base::client,
      beast::bind_front_handler(
        &WebSocket::onSSLHandshake, shared_from_this())
      );
}

void WebSocket::onSSLHandshake(beast::error_code ec) {
  if (ec) {
    return fail(ec, "SSL");
  }

  beast::get_lowest_layer(m_ws).expires_never();

  m_ws.set_option(
      websocket::stream_base::timeout::suggested(
        beast::role_type::client));

   m_ws.set_option(websocket::stream_base::decorator(
    [](websocket::request_type& req) {
      req.set(http::field::user_agent,
        std::string(BOOST_BEAST_VERSION_STRING) +
        " websocket-client-async-ssl");
      }));

  m_ws.async_handshake(m_host, "/v2",
    beast::bind_front_handler(
    &WebSocket::onHandshake, shared_from_this()));
}

void WebSocket::onHandshake(beast::error_code ec) {
  if (ec) {
    return fail(ec, "handshake");
  }
  
  json subscribe_msg = {
    {"method", "subscribe"},
    {"params", {
      {"channel", m_channel},
      {"symbol", {m_symbol}}
    }}
  };

  send(subscribe_msg);
}

void WebSocket::onRead(beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec) {
    return fail(ec, "on read");
  }

  std::cout << "Received: " << beast::make_printable(m_buffer.data()) << std::endl;
  m_buffer.consume(m_buffer.size());

  doRead();
}

void WebSocket::doRead() {
  m_ws.async_read(m_buffer, 
      beast::bind_front_handler(&WebSocket::onRead, shared_from_this()));
}

void WebSocket::onWrite(beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);
  if (ec) {
    return fail(ec, "on write");
  }

  doRead();
}

void WebSocket::send(json& msg) {
  m_ws.async_write(net::buffer(msg.dump()),
      beast::bind_front_handler(&WebSocket::onWrite, shared_from_this()));
}

void WebSocket::onUnsubscribe(beast::error_code ec, std::size_t bytes_transferred) {
  if (ec) {
    return fail(ec, "unsubcribe");
  }

  m_ws.async_close(beast::websocket::close_code::normal, 
      beast::bind_front_handler(&WebSocket::onClose, shared_from_this())
      );
}

void WebSocket::onClose(beast::error_code ec) {
  if (ec) {
    return fail(ec, "close");
  }

  std::cout << "Websocket for " << m_channel << ": " << m_symbol << " has closed." << std::endl;
}

