#pragma once

#include "../model/game.h"
#include "../http_server/http_server.h"
#include "api_request_handler.h"
#include "static_file_request_handler.h"


namespace http_handler {


class RequestHandler {
public:
  explicit RequestHandler(application::Application& app, std::string static_file_dir)
    : api_handler_{ app }
    , static_file_handler_(std::move(static_file_dir)) {
  }

  RequestHandler(const RequestHandler&) = delete;
  RequestHandler& operator=(const RequestHandler&) = delete;

  template <typename Body, typename Allocator, typename Send>
  void operator()(const boost::asio::ip::tcp::endpoint& endpoint,
    http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {

    if (req.target().starts_with(config::Endpoint::API)) {

      return api_handler_.HandleApiRequest(std::move(req), std::forward<Send>(send));

    }
    else {

      return static_file_handler_.HandleStaticFileRequest(std::move(req), std::forward<Send>(send));

    }

  }

private:

  ApiRequestHandler api_handler_;
  StaticFileHandler static_file_handler_;

};
}