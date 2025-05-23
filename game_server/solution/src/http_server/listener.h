﻿#pragma once

#include "../sdk.h"
#define BOOST_BEAST_USE_STD_STRING_VIEW


#include "../logger/logger.h"
#include "session.h"


namespace http_server {

namespace sys = boost::system;

template <typename RequestHandler>
class Listener : public std::enable_shared_from_this<Listener<RequestHandler>> {
public:

  template <typename Handler>
  Listener(net::io_context& ioc, const tcp::endpoint& endpoint, Handler&& request_handler)
    : ioc_(ioc)
    , acceptor_(net::make_strand(ioc))
    , request_handler_(std::forward<Handler>(request_handler)) {
    acceptor_.open(endpoint.protocol());

    acceptor_.set_option(net::socket_base::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen(net::socket_base::max_listen_connections);
  }

  void Run() {
    DoAccept();
  }


private:

  void DoAccept() {

    acceptor_.async_accept(

      net::make_strand(ioc_),

      beast::bind_front_handler(&Listener::OnAccept, this->shared_from_this()));
  }

  void OnAccept(sys::error_code ec, tcp::socket socket) {

    using namespace std::literals;

    if (ec) {
      return logger::ReportError(ec, "accept"sv);
    }


    AsyncRunSession(std::move(socket));

    DoAccept();
  }

  void AsyncRunSession(tcp::socket&& socket) {
    std::make_shared<Session<RequestHandler>>(std::move(socket), request_handler_)->Run();
  }

  net::io_context& ioc_;
  tcp::acceptor acceptor_;
  RequestHandler request_handler_;

};

}