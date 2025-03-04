#include "sdk.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <iostream>
#include <thread>

#include "json_loader/json_loader.h"
#include "request/request_handler.h"
#include "logger/logger.h"
#include "application/application.h"
#include "parse_command_line/parse_command_line.h"


using namespace std::literals;
namespace net = boost::asio;

namespace {


template <typename Fn>
void RunWorkers(unsigned num_workers, const Fn& fn) {
  num_workers = std::max(1u, num_workers);
  std::vector<std::jthread> workers;
  workers.reserve(num_workers - 1);
  while (--num_workers) {
    workers.emplace_back(fn);
  }
  fn();
}

}

int main(int argc, const char* argv[]) {

  logger::InitBoostLogFilter();

  try {

    if (auto args = parse_command::ParseCommandLine(argc, argv)) {

      const auto db_url = std::getenv(std::string(config::DataBase::DATADASE_URL).c_str());

      if (!db_url) {
        throw std::runtime_error("DATADASE_URL environment variable is not set");
      }

      const unsigned num_threads = std::thread::hardware_concurrency();
      net::io_context ioc(num_threads);
      
      model::Game game = json_loader::LoadGame(args->config_file);

      application::Application app{ game, ioc , 
        std::chrono::milliseconds(args->tick_period), args->randomize_spawn_points,
        args->state_file, std::chrono::milliseconds(args->save_state_period), num_threads, db_url };
     
      net::signal_set signals(ioc, SIGINT, SIGTERM);
      signals.async_wait([&ioc](const boost::system::error_code& ec, [[maybe_unused]] int signal_number) {
        if (!ec) {
          ioc.stop();
        }
        });
      http_handler::RequestHandler handler{ app, args->www_root };

      logger_request::LoggingRequestHandler<http_handler::RequestHandler> logger_handler{ handler };

      const auto address = net::ip::make_address("0.0.0.0");
      constexpr net::ip::port_type port = 8080;

      http_server::ServeHttp(ioc, { address, port }, [&logger_handler](auto&& endpoint, auto&& req, auto&& send) {
        logger_handler(std::forward<decltype(endpoint)>(endpoint),
          std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
        });


      logger::LogStart(address.to_string(), port);


      RunWorkers(std::max(1u, num_threads), [&ioc] {
        ioc.run();
        });
      
      app.SaveStateGame();

      logger::LogExit(0);
      return EXIT_SUCCESS;
    }
  }
  catch (const std::exception& ex) {
    logger::LogStop(EXIT_FAILURE, ex);
    return EXIT_FAILURE;
  }
}
