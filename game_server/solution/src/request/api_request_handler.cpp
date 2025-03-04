#include "api_request_handler.h"


namespace http_handler {

StringResponse ApiRequestHandler::CreateJsonResponse(const http::request<http::string_body>& req,
  http::status status,
  const json::value& value) {

  return http_handler::MakeStringResponse(status, json::serialize(value),
    req.version(), req.keep_alive(), config::ContentType::APPLICATION_JSON);

}

application::Token ApiRequestHandler::GetToken(const http::request<http::string_body>& req) {

  auto auth_header = req.find(http::field::authorization);
  std::string auth_value = std::string(auth_header->value());
  std::string token_str = auth_value.substr(7);
  return application::Token{ token_str };

}

void ApiRequestHandler::Move(std::string move_value, application::Token token) {

  model::Direction direction;

  if (move_value == config::MapKey::LEFT) {
    direction = model::Direction::WEST;
  }
  else if (move_value == config::MapKey::RIGHT) {
    direction = model::Direction::EAST;
  }
  else if (move_value == config::MapKey::UP) {
    direction = model::Direction::NORTH;
  }
  else if (move_value == config::MapKey::DOWN) {
    direction = model::Direction::SOUTH;
  }
  else direction = model::Direction::NONE;

  app_.MovePlayer(token, direction);

}

json::value ApiRequestHandler::SerializeMapsList() const {
  json::array result;
  for (const auto& map : app_.GetMapList()) {
    json::object map_obj;
    map_obj[std::string(config::MapKey::ID)] = *map.get()->GetId();
    map_obj[std::string(config::MapKey::NAME)] = map.get()->GetName();
    result.push_back(map_obj);
  }
  return result;
}

json::value ApiRequestHandler::SerializeMapInfo(std::string_view id) {
  if (const auto map = app_.FindMap(model::Map::Id{ std::string{id} })) {

    json::object result;
    result[std::string(config::MapKey::ID)] = std::string(id);
    result[std::string(config::MapKey::NAME)] = map->GetName();

    json::array roads;
    for (const auto& road : map->GetRoads()) {
      json::object road_obj;
      road_obj[std::string(config::MapKey::START_X)] = road.GetStart().x;
      road_obj[std::string(config::MapKey::START_Y)] = road.GetStart().y;
      if (road.IsHorizontal()) {
        road_obj[std::string(config::MapKey::END_X)] = road.GetEnd().x;
      }
      else {
        road_obj[std::string(config::MapKey::END_Y)] = road.GetEnd().y;
      }
      roads.push_back(road_obj);
    }
    result[std::string(config::MapKey::ROADS)] = roads;

    json::array buildings;
    for (const auto& building : map->GetBuildings()) {
      json::object building_obj;
      const auto& rect = building.GetBounds();
      building_obj[std::string(config::MapKey::POSITION_X)] = rect.position.x;
      building_obj[std::string(config::MapKey::POSITION_Y)] = rect.position.y;
      building_obj[std::string(config::MapKey::WIDTH)] = rect.size.width;
      building_obj[std::string(config::MapKey::HEIGHT)] = rect.size.height;
      buildings.push_back(building_obj);
    }
    result[std::string(config::MapKey::BUILDINGS)] = buildings;

    json::array offices;
    for (const auto& office : map->GetOffices()) {
      json::object office_obj;
      office_obj[std::string(config::MapKey::ID)] = *office.GetId();
      office_obj[std::string(config::MapKey::POSITION_X)] = office.GetPosition().x;
      office_obj[std::string(config::MapKey::POSITION_Y)] = office.GetPosition().y;
      office_obj[std::string(config::MapKey::OFFSET_X)] = office.GetOffset().dx;
      office_obj[std::string(config::MapKey::OFFSET_Y)] = office.GetOffset().dy;
      offices.push_back(office_obj);
    }
    result[std::string(config::MapKey::OFFICES)] = offices;

    const auto& loot_types = map->GetLootTypes();

    json::array array_loot_types;
    for (const auto& loot_type : loot_types) {
    json::object object{
        {std::string(config::Loot::NAME), loot_type.name},
        {std::string(config::Loot::FILE), loot_type.file},
        {std::string(config::Loot::TYPE), loot_type.type},
        {std::string(config::Loot::SCALE), loot_type.scale},
        {std::string(config::Loot::VALUE), loot_type.value}
    };

    if (loot_type.rotation) {
      object[std::string(config::Loot::ROTATION)] = *loot_type.rotation;
    }

    if (loot_type.color) {
      object[std::string(config::Loot::COLOR)] = *loot_type.color;
    }

    array_loot_types.push_back(std::move(object));
    }

    result[std::string(config::Loot::LOOT_TYPES)] = array_loot_types;
   
    return result;
  }
  return json::object{ { std::string(config::ErrorInfo::CODE), config::Code::MAP_NOT_FOUND},
    {std::string(config::ErrorInfo::MESSAGE), "Map not found"} };
}

json::value ApiRequestHandler::SerializeMessage(std::string_view code, std::string_view message) {

  json::object result;
  result[std::string(config::ErrorInfo::CODE)] = std::string(code);
  result[std::string(config::ErrorInfo::MESSAGE)] = std::string(message);

  return result;

}

json::value ApiRequestHandler::ApiRequestHandler::SerializePlayer(application::JoinGameData join_result) {

  json::object result;
  result[std::string(config::MapKey::AUTH_TOKEN)] = *join_result.token;
  result[std::string(config::MapKey::PLAYER_ID)] = *join_result.id;

  return result;

}

json::value ApiRequestHandler::SerializePlayers(const Players& players) {

  json::object result;

  for (const auto& player : players) {

    if (auto p = player) {
      result[std::to_string(*p->GetId())] = { { std::string(config::MapKey::NAME), p->GetName()} };
    }

  }
  return result;
}

json::value ApiRequestHandler::SerializePlayersInfoAndLostObject(const Players& players, const std::vector<model::LostObject>& lost_objects) {

  json::object players_obj;

  for (const auto& player : players) {

    if (auto p = player) {
      json::object player_info;

      player_info[std::string(config::MapKey::POSITION)] = { p->GetDog()->GetPosition().x, p->GetDog()->GetPosition().y };
      player_info[std::string(config::MapKey::SPEED)] = { p->GetDog()->GetSpeed().x, p->GetDog()->GetSpeed().y };
      
      auto dir = p->GetDog()->GetDirection();

      std::string dir_str;

      if (dir == model::Direction::EAST) {
        dir_str = std::string(config::MapKey::RIGHT);
      }
      else if (dir == model::Direction::NORTH) {
        dir_str = std::string(config::MapKey::UP);
      }
      else if (dir == model::Direction::SOUTH) {
        dir_str = std::string(config::MapKey::DOWN);
      }
      else if (dir == model::Direction::WEST) {
        dir_str = std::string(config::MapKey::LEFT);
      }
      else {
        dir_str = "";
      }
      player_info[std::string(config::MapKey::DIRECTION)] = dir_str;

      auto bag = p->GetDog()->GetBag();

      json::array array_bag;
      for (const auto& object : bag) {
        array_bag.push_back(json::object{
          {std::string(config::MapKey::ID), *object.GetId()},
          {std::string(config::MapKey::TYPE), object.GetType()},
          });
      
      }

      player_info[std::string(config::MapKey::BAG)] = array_bag;

      player_info[std::string(config::MapKey::SCORE)] = p->GetDog()->GetScore();

      players_obj[std::to_string(*p->GetId())] = player_info;
    }
  }

  json::object lost_obj;
  for (const auto& lost_object : lost_objects) {
    lost_obj[std::to_string(*lost_object.GetId())] = json::object{
      {std::string(config::Loot::TYPE), lost_object.GetType()},
      {std::string(config::MapKey::POSITION), json::array{lost_object.GetPosition().x, lost_object.GetPosition().y}}
    };
  
  }

  return json::object{ 
    {std::string(config::MapKey::PLAYERS) , players_obj},
    {std::string(config::Loot::LOST_OBJECTS) , lost_obj}
  };
}

json::value ApiRequestHandler::SerializePlayersRecords(const std::vector<db_domain::PlayerRecord>& records) {

  json::array players_records;

  for (const auto& record : records) {
    players_records.push_back(json::object{
      {std::string(config::Records::NAME), record.GetName()},
      {std::string(config::Records::SCORE), record.GetScore()},
      {std::string(config::Records::PLAY_TIME), record.GetPlayTime().count()/model::MILLISECONDS_IN_SECOND}
      });
  }

  return players_records;

}

}