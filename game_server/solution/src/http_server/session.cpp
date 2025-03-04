#include "session.h"
#include "../logger/logger.h"

namespace http_server {


void SessionBase::Run()
{
  net::dispatch(stream_.get_executor(),
    beast::bind_front_handler(&SessionBase::Read, GetSharedThis()));
}

void SessionBase::Read()
{
  using namespace std::literals;
  request_ = {};
  stream_.expires_after(30s);
  http::async_read(stream_, buffer_, request_,
    beast::bind_front_handler(&SessionBase::OnRead, GetSharedThis()));
}

void SessionBase::OnRead(beast::error_code ec, std::size_t bytes_read)
{
  using namespace std::literals;
  if (ec == http::error::end_of_stream) {
    return Close();
  }
  if (ec) {
    return logger::ReportError(ec, "read"sv);
  }
  HandleRequest(std::move(request_));
}

void SessionBase::Close()
{
  beast::error_code ec;
  stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
  if (ec) {
    logger::ReportError(ec, "Shutdown"sv);
  }
}

void SessionBase::OnWrite(bool close, beast::error_code ec, std::size_t bytes_written)
{
  if (ec) {
    return logger::ReportError(ec, "write"sv);
  }

  if (close) {

    return Close();
  }

  Read();
}

tcp::endpoint SessionBase::GetEndpoint() const {
  return stream_.socket().remote_endpoint();
}

}