#pragma once

#include <boost/json.hpp>

#include <filesystem>
#include <fstream>

#include "../model/game.h"


namespace json_loader {

model::Game LoadGame(const std::filesystem::path& json_path);

}  