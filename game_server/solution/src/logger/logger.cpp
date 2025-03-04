#include "logger.h"



namespace logger {

void ReportError(beast::error_code ec, std::string_view what) {

  LogError(ec, what);

}

void LogFormatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm) {
  auto ts = *rec[timestamp];
  strm << "{\"timestamp\":\"" << to_iso_extended_string(ts) << "\",";

  auto json_data = boost::json::serialize(*rec[additional_data]);

  strm << "\"data\":" << json_data << ",";

  strm << "\"message\":\"" << rec[boost::log::expressions::smessage] << "\"}";
}

void InitBoostLogFilter() {
  boost::log::add_common_attributes();
  boost::log::add_console_log(
    std::cout,
    boost::log::keywords::format = &LogFormatter,
    boost::log::keywords::auto_flush = true
  );
  boost::log::core::get()->set_filter(
    boost::log::trivial::severity >= boost::log::trivial::info
  );
}

void LogError(const boost::system::error_code& ec, std::string_view where) {
  boost::json::object data = {
    {"code", ec.value()},
    {"text", ec.message()},
    {"where", where},
  };
  BOOST_LOG_TRIVIAL(error) << boost::log::add_value(additional_data, data) << "error";
}

void LogStop(int code, const std::exception& exception) {
  const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
  boost::json::object data{
    {"code", code },
    {"exception", exception.what()}
  };
  BOOST_LOG_TRIVIAL(error) << boost::log::add_value(additional_data, data) << "server stoped"sv;
}

void LogExit(const int& code) {
  boost::json::object data{ {"code", code} };
  if (code)
    BOOST_LOG_TRIVIAL(error) << boost::log::add_value(additional_data, data) << "server exited"sv;
  else
    BOOST_LOG_TRIVIAL(info) << boost::log::add_value(additional_data, data) << "server exited"sv;
}

void LogStart(std::string address, unsigned port) {
  boost::json::object data{
    {"port", port},
    {"address", address}
  };
  BOOST_LOG_TRIVIAL(info) << boost::log::add_value(additional_data, data) << "server has started"sv;
}
}