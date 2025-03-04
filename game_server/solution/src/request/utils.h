#pragma once
#define BOOST_BEAST_USE_STD_STRING_VIEW


#include <boost/beast.hpp>

#include "../constants/config.h"


namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;

using FileResponse = http::response<http::file_body>;
using StringResponse = http::response<http::string_body>;

inline StringResponse MakeStringResponse(http::status status,
  std::string_view body,
  unsigned http_version,
  bool keep_alive,
  std::string_view content_type,
  std::string_view cache_control = config::NO_CACHE) {
  StringResponse response(status, http_version);
  response.set(http::field::content_type, content_type);
  response.set(http::field::cache_control, cache_control);
  response.body() = std::string(body);
  response.content_length(body.size());
  response.keep_alive(keep_alive);
  return response;
}

}