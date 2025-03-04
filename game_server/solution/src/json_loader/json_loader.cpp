#include "json_loader.h"
#include "../constants/config.h"


#include <fstream>
#include <boost/json.hpp>
#include "../logger/logger.h"
#include "../model/loot/loot_generator.h"
#include "../model/data_model.h"
#include <iostream>




namespace json_loader {

namespace json = boost::json;

std::string ReadFile(const std::filesystem::path& json_path) {

  std::ifstream file(json_path);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the file!");
  }

  std::string tp;
  std::string text;
  while (getline(file, tp)) {
    text += tp;
  }
  file.close();
  return text;
};

void ApplyRoadsMap(model::Map& map, const json::value& map_object) {

  if (!map_object.as_object().contains(std::string(config::MapKey::ROADS))) {
    return;
  }

  for (auto object : map_object.as_object().at(std::string(config::MapKey::ROADS)).as_array()) {

    model::Coord x{ static_cast<int>(object.as_object().at(std::string(config::MapKey::START_X)).as_int64()) };
    model::Coord y{ static_cast<int>(object.as_object().at(std::string(config::MapKey::START_Y)).as_int64()) };
    model::Point startPoint(x, y);
    if (object.as_object().contains(std::string(config::MapKey::END_X))) {
      model::Coord end{ static_cast<int>(object.as_object().at(std::string(config::MapKey::END_X)).as_int64()) };
      map.AddRoad(model::Road(model::Road::HORIZONTAL, startPoint, end));
    }
    else {
      model::Coord end{ static_cast<int>(object.as_object().at(std::string(config::MapKey::END_Y)).as_int64()) };
      map.AddRoad(model::Road(model::Road::VERTICAL, startPoint, end));
    }
  }
};

void ApplyBuildingsMap(model::Map& map, const json::value& map_object) {
  if (!map_object.as_object().contains(std::string(config::MapKey::BUILDINGS))) {
    return;
  }
  for (auto object : map_object.as_object().at(std::string(config::MapKey::BUILDINGS)).as_array()) {
    model::Coord x{ static_cast<int>(object.as_object().at(std::string(config::MapKey::POSITION_X)).as_int64()) };
    model::Coord y{ static_cast<int>(object.as_object().at(std::string(config::MapKey::POSITION_Y)).as_int64()) };
    model::Coord w{ static_cast<int>(object.as_object().at(std::string(config::MapKey::WIDTH)).as_int64()) };
    model::Coord h{ static_cast<int>(object.as_object().at(std::string(config::MapKey::HEIGHT)).as_int64()) };
    model::Rectangle rect{ model::Point{x, y}, model::Size{w, h} };
    map.AddBuilding(model::Building(std::move(rect)));
  }
};

void ApplyOfficesMap(model::Map& map, const json::value& map_object) {
  if (!map_object.as_object().contains(std::string(config::MapKey::OFFICES))) {
    return;
  }
  for (auto object : map_object.as_object().at(std::string(config::MapKey::OFFICES)).as_array()) {
    model::Office::Id id_office(std::string(object.as_object().at(std::string(config::MapKey::ID)).as_string()));
    model::Coord x{ static_cast<int>(object.as_object().at(std::string(config::MapKey::POSITION_X)).as_int64()) };
    model::Coord y{ static_cast<int>(object.as_object().at(std::string(config::MapKey::POSITION_Y)).as_int64()) };
    model::Coord dx{ static_cast<int>(object.as_object().at(std::string(config::MapKey::OFFSET_X)).as_int64()) };
    model::Coord dy{ static_cast<int>(object.as_object().at(std::string(config::MapKey::OFFSET_Y)).as_int64()) };
    map.AddOffice(model::Office(id_office,
      model::Point(x, y),
      model::Offset(dx, dy)));
  }
};

double ApplyDogSpeed(const json::value& object, const std::string& key, double default_value) {
  if (object.as_object().contains(key)) {
    return object.as_object().at(key).as_double();
  }
  else {
    return default_value;
  }
}

size_t ApplyBagCapacity(const json::value& object, const std::string& key, size_t default_value) {
  if (object.as_object().contains(key)) {
    return static_cast<size_t>(object.as_object().at(key).as_uint64());
  }
  else {
    return default_value;
  }

}

std::pair<loot_gen::LootGenerator::TimeInterval, double> ApplyLootGenerator(const json::value& object) {

  auto config = object.at(std::string(config::Loot::LOOT_GEN_CONFIG)).as_object();
 
  loot_gen::LootGenerator::TimeInterval time_interval = loot_gen::LootGenerator::TimeInterval(
    static_cast<uint64_t>(config.at(std::string(config::Loot::PERIOD)).as_double() * model::MILLISECONDS_IN_SECOND));
  
    double probability = config.at(std::string(config::Loot::PROBABILITY)).as_double();
   
    return std::make_pair(time_interval, probability);
}

void ApplyLootType(model::Map& map, const json::value& map_object) {

  auto loot_type_obj = map_object.as_object().at(std::string(config::Loot::LOOT_TYPES));

  for (auto loot : loot_type_obj.as_array()) {

    model::LootType loot_type{
          .name = std::string(loot.at(std::string(config::Loot::NAME)).as_string()),
          .file = std::string(loot.at(std::string(config::Loot::FILE)).as_string()),
          .type = std::string(loot.at(std::string(config::Loot::TYPE)).as_string()),
          .scale = loot.at(std::string(config::Loot::SCALE)).as_double(),
          .value = loot.at(std::string(config::Loot::VALUE)).as_int64(),
    };
   
    if (loot.as_object().contains(std::string(config::Loot::ROTATION))) {
      loot_type.rotation = loot.at(std::string(config::Loot::ROTATION)).as_int64();
    }
   

    if (loot.as_object().contains(std::string(config::Loot::COLOR))) {
      loot_type.color = std::string(loot.at(std::string(config::Loot::COLOR)).as_string());
    }
    
    map.AddLootType(loot_type);
  }
 
}

std::chrono::milliseconds AppplyDogRetiermentTime(const json::value& object, 
  const std::string& key, std::chrono::milliseconds default_value) {

  if (object.as_object().contains(key)) {
    return std::chrono::milliseconds(static_cast<uint64_t>(object.as_object().at(key).as_double()))*static_cast<uint64_t>(model::MILLISECONDS_IN_SECOND);
  }
  else {
    return default_value;
  }

}


model::Game LoadGame(const std::filesystem::path& json_path) {

  std::string text{ ReadFile(json_path) };

  auto map_json = json::parse(text);
  
  auto [time_interval, probability] = ApplyLootGenerator(map_json);

  auto max_inactive_time = AppplyDogRetiermentTime(map_json, std::string(config::MapKey::DOG_RETIREMENT_TIME), 
    model::DEFAULT_DOG_RETIREMENT_TIME);
  model::Game game{time_interval, probability, max_inactive_time};
 
  double default_dog_speed = ApplyDogSpeed(map_json, 
    std::string(config::MapKey::DEFAULT_DOG_SPEED), model::DEFAULT_DOG_SPEED);
  
  size_t default_bag_capacity = ApplyBagCapacity(map_json, 
    std::string(config::MapKey::DEFAULT_BAG_CAPACITY), model::DEFAULT_BAG_CAPACITY);

  for (const auto& map_object : map_json.as_object().at(std::string(config::MapKey::MAPS)).as_array()) {
    std::string id(map_object.as_object().at(std::string(config::MapKey::ID)).as_string());
    std::string name(map_object.as_object().at(std::string(config::MapKey::NAME)).as_string());
    double dog_speed = ApplyDogSpeed(map_object, std::string(config::MapKey::DOG_SPEED),
      default_dog_speed);

    size_t bag_capacity = ApplyBagCapacity(map_object, 
      std::string(config::MapKey::BAG_CAPACITY), default_bag_capacity);
    model::Map map(model::Map::Id(id), name, dog_speed, bag_capacity);
    
    ApplyRoadsMap(map, map_object);
    ApplyBuildingsMap(map, map_object);
    ApplyOfficesMap(map, map_object);
    ApplyLootType(map, map_object);
    
    game.AddMap(std::move(map));
  }
  return game;
};

}