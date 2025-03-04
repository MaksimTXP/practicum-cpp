#pragma once

#include <memory>
#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <functional>
#include <cassert>
#include <iostream>

#include "../logger/logger.h"


namespace http_server {

namespace net = boost::asio;
namespace sys = boost::system;

class Ticker : public std::enable_shared_from_this<Ticker> {
public:
  using Strand = net::strand<net::io_context::executor_type>;
  using Handler = std::function<void(std::chrono::milliseconds tick_duration)>;

  Ticker(std::shared_ptr<Strand> strand, std::chrono::milliseconds period, Handler handler)
    : strand_{ strand }, period_{ period }, handler_{ std::move(handler) }, timer_{ *strand } {
    last_tick_ = Clock::now();
  }

  void Start() {
    ScheduleTick();
  }

private:
  void ScheduleTick() {
    
    timer_.expires_after(period_);
    timer_.async_wait([self = shared_from_this()](sys::error_code ec) {
      self->OnTick(ec);
      });
    
  }

  void OnTick(sys::error_code ec) {
    using namespace std::chrono;
    assert(strand_->running_in_this_thread());
    if (!ec) {
      auto this_tick = Clock::now();
      auto tick_duration = duration_cast<milliseconds>(this_tick - last_tick_);
      last_tick_ = this_tick;
      try {
        handler_(tick_duration);
      }
      catch (const std::exception& e) {
        logger::LogError(ec, "Error in the handler: " + std::string(e.what())); 
      }
      catch (...) {
        logger::LogError(ec, "Unknown error in the handler"); 
      }
      ScheduleTick();
    }
  }

  using Clock = std::chrono::steady_clock;

  std::shared_ptr<Strand> strand_;
  std::chrono::milliseconds period_;
  net::steady_timer timer_;
  Handler handler_;
  std::chrono::time_point<Clock> last_tick_;
};
}
