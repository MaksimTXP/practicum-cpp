#pragma once

#include "../sdk.h"

#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http.hpp>
#include <boost/date_time.hpp>
#include <boost/json.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>

#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

BOOST_LOG_ATTRIBUTE_KEYWORD(additional_data, "AdditionalData", boost::json::value);
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime);

using namespace std::literals;
namespace http = boost::beast::http;
namespace beast = boost::beast;

namespace logger {

void ReportError(beast::error_code ec, std::string_view what);

void LogFormatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm);

void InitBoostLogFilter();

void LogError(const boost::system::error_code& ec, std::string_view where);

void LogStop(int code, const std::exception& exception);

void LogExit(const int& code);

void LogStart(std::string address, unsigned port);
}

namespace logger_request {

template <class SomeRequestHandler>
class LoggingRequestHandler {

  template <typename Body, typename Allocator>
  static void LogRequest(const boost::asio::ip::tcp::endpoint& endpoint,
    http::request<Body, http::basic_fields<Allocator>>& req) {

    boost::json::object data{
      {"ip", endpoint.address().to_string()},
      {"URI", req.target()},
      {"method", req.method_string()}
    };

    Log("request received", data);
  }

  static void LogResponse(int delta, int code, std::string content) {
    if (content.empty()) {
      content = "null";
    }
    boost::json::object data{
      {"response_time", delta},
      {"code", code},
      {"content_type", content}
    };

    Log("response sent", data);
  }

public:
  explicit LoggingRequestHandler(SomeRequestHandler& decorated) : decorated_(decorated) {}

  template <typename Body, typename Allocator, typename Send>
  void operator()(const boost::asio::ip::tcp::endpoint& endpoint, http::request<Body, http::basic_fields<Allocator>>&& req,
    Send&& send) {

    LogRequest(endpoint, req);

    decorated_(endpoint, std::move(req), [response_sender = std::move(send)](auto&& response) {
      std::chrono::high_resolution_clock timer;
      auto start = timer.now();

      const int code_result = response.result_int();
      const std::string content_type = static_cast<std::string>(response.at(http::field::content_type));
      response_sender(response);

      auto stop = timer.now();
      int time = static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count());

      LoggingRequestHandler::LogResponse(time, response.result_int(), content_type);
      });

    return;
  }

  static void Log(const std::string& message, const boost::json::object& data) {
    BOOST_LOG_TRIVIAL(info) << boost::log::add_value(additional_data, data) << message;
  }

private:

  SomeRequestHandler& decorated_;
};

}