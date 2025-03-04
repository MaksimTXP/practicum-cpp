#pragma once
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/beast.hpp>
#include <filesystem>
#include "utils.h"
#include "../constants/config.h"

namespace http_handler {

using namespace std::literals;
namespace sys = boost::system;
namespace fs = std::filesystem;

class StaticFileHandler {
public:
  explicit StaticFileHandler(std::string static_file_dir)
    : static_file_dir_(std::move(static_file_dir)) {
  }

  template<typename Body, typename Allocator, typename Send>
  void HandleStaticFileRequest(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {

    const auto handle_error_response = [&](http::status status, std::string_view message) {
      return send(MakeStringResponse(status, message, req.version(), req.keep_alive(),
        config::ContentType::TEXT_PLAIN));
      };

    fs::path path = fs::weakly_canonical(static_file_dir_ + UrlDecode(req.target()));

    if (fs::is_directory(path)) {
      path /= "index.html";
    }

    if (!IsSubPathOfRoot(path, static_file_dir_)) {
      return handle_error_response(http::status::bad_request, "Incorrect URL");
    }

    sys::error_code ec;
    http::file_body::value_type file;
    file.open(path.string().c_str(), beast::file_mode::read, ec);
    if (ec) {
      return handle_error_response(http::status::not_found, "This page does not exist");
    }

    http::file_body::value_type file_body = std::move(file);

    http::response<http::file_body> res(http::status::ok, req.version());
    res.set(http::field::content_type, GetMimeType(path.extension().string()));
    res.body() = std::move(file_body);
    res.prepare_payload();
    res.keep_alive(req.keep_alive());

    return send(std::move(res));
  }

private:

  std::string GetMimeType(const std::string& extension);

  std::string UrlDecode(std::string_view str);

  bool IsSubPathOfRoot(fs::path path, fs::path base);

  std::string static_file_dir_;
};

}
