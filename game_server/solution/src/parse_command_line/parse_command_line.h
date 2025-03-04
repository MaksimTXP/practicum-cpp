#pragma once 

#include <boost/program_options.hpp>

#include <fstream>
#include <iostream>
#include <optional>
#include <vector>


namespace parse_command {


struct Args {
  size_t tick_period{ 0 };
  std::string config_file;
  std::string www_root;
  bool randomize_spawn_points{ false };
  std::string state_file;
  size_t save_state_period{ 0 };
};

[[nodiscard]] std::optional<Args> ParseCommandLine(int argc, const char* const argv[]) {
  namespace po = boost::program_options;

  po::options_description desc{ "All options" };

  Args args;

  desc.add_options()
    ("help,h", "produce help message")

    ("tick-period,t", po::value(&args.tick_period)->value_name("milliseconds"), "set tick period")

    ("config-file,c", po::value(&args.config_file)->value_name("file"), "set config file path")

    ("www-root,w", po::value(&args.www_root)->value_name("dir"), "set static files root")

    ("randomize-spawn-points", po::value(&args.randomize_spawn_points), "spawn dogs at random positions")

    ("state-file", po::value(&args.state_file)->value_name("file"), "set state file path")

    ("save-state-period", po::value(&args.save_state_period)->value_name("milliseconds"), "set save state period");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.contains("help")) {

    std::cout << desc;
    return std::nullopt;
  }
  if (!vm.contains("config-file")) {
    throw std::runtime_error("game-config-json is missing");
  }
  if (!vm.contains("www-root")) {
    throw std::runtime_error("static-root is missing");
  }

  return args;
}

}