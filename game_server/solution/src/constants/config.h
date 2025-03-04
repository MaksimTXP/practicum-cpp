#pragma once

#include <string_view>

#include <unordered_map>


namespace config {

using namespace std::literals;

struct MapKey {

  MapKey() = delete;

  static constexpr std::string_view AUTH_TOKEN = "authToken"sv;
  static constexpr std::string_view BUILDINGS = "buildings"sv;
  static constexpr std::string_view DEFAULT_DOG_SPEED = "defaultDogSpeed"sv;
  static constexpr std::string_view DIRECTION = "dir"sv;
  static constexpr std::string_view DOG_SPEED = "dogSpeed"sv;
  static constexpr std::string_view DOWN = "D"sv;
  static constexpr std::string_view END_X = "x1"sv;
  static constexpr std::string_view END_Y = "y1"sv;
  static constexpr std::string_view HEIGHT = "h"sv;
  static constexpr std::string_view ID = "id"sv;
  static constexpr std::string_view LEFT = "L"sv;
  static constexpr std::string_view MAP = "map"sv;
  static constexpr std::string_view MAP_ID = "mapId"sv;
  static constexpr std::string_view MAPS = "maps"sv;
  static constexpr std::string_view MOVE = "move"sv;
  static constexpr std::string_view NAME = "name"sv;
  static constexpr std::string_view OFFSET_X = "offsetX"sv;
  static constexpr std::string_view OFFSET_Y = "offsetY"sv;
  static constexpr std::string_view OFFICES = "offices"sv;
  static constexpr std::string_view PLAYER_ID = "playerId"sv;
  static constexpr std::string_view PLAYERS = "players"sv;
  static constexpr std::string_view POSITION = "pos"sv;
  static constexpr std::string_view POSITION_X = "x"sv;
  static constexpr std::string_view POSITION_Y = "y"sv;
  static constexpr std::string_view RIGHT = "R"sv;
  static constexpr std::string_view ROADS = "roads"sv;
  static constexpr std::string_view SPEED = "speed"sv;
  static constexpr std::string_view START_X = "x0"sv;
  static constexpr std::string_view START_Y = "y0"sv;
  static constexpr std::string_view TIME_DELTA = "timeDelta"sv;
  static constexpr std::string_view UP = "U"sv;
  static constexpr std::string_view USER_NAME = "userName"sv;
  static constexpr std::string_view WIDTH = "w"sv;
  static constexpr std::string_view DEFAULT_BAG_CAPACITY = "defaultBagCapacity"sv;
  static constexpr std::string_view BAG_CAPACITY = "bagCapacity"sv;
  static constexpr std::string_view TYPE = "type"sv;
  static constexpr std::string_view BAG = "bag"sv;
  static constexpr std::string_view SCORE = "score"sv;
  static constexpr std::string_view DOG_RETIREMENT_TIME = "dogRetirementTime"sv;
};

static const std::unordered_map<std::string, std::string> MIME_TYPE = {
  {".htm", "text/html"}, {".html","text/html"}, {".css", "text/css"},
  {".txt","text/plain"}, {".js", "text/javascript"}, {".json", "application/json"},
  {".xml","application/xml"}, {".png", "image/png"} , {".jpg","image/jpeg"},
  {".jpe","image/jpeg"}, {".jpeg","image/jpeg"}, {".gif", "image/gif"},
  {".bmp","image/bmp"}, {".ico","image/vnd.microsoft.icon"},
  {".tiff","image/tiff"}, {".tif","image/tiff"}, {".svg","image/svg+xml"},
  {".svgz","image/svg+xml"}, {".mp3","audio/mpeg"}
};

struct ContentType {

  ContentType() = delete;

  static constexpr std::string_view APPLICATION_JSON = "application/json"sv;
  static constexpr std::string_view TEXT_PLAIN = "text/plain"sv;
  static constexpr std::string_view APPLICATION_OCTET_STREAM = "application/octet-stream"sv;

};

struct Endpoint {

  Endpoint() = delete;

  static constexpr std::string_view API = "/api/"sv;
  static constexpr std::string_view GAME = "/api/v1/game/"sv;
  static constexpr std::string_view JOIN_GAME = "/api/v1/game/join"sv;
  static constexpr std::string_view PLAYERS_LIST = "/api/v1/game/players"sv;
  static constexpr std::string_view MAPS_LIST = "/api/v1/maps"sv;
  static constexpr std::string_view GAME_STATE = "/api/v1/game/state"sv;
  static constexpr std::string_view ACTION = "/api/v1/game/player/action"sv;
  static constexpr std::string_view TICK = "/api/v1/game/tick"sv;
  static constexpr std::string_view RECORDS = "/api/v1/game/records"sv;
};

struct RequestMethod {

  RequestMethod() = delete;

  static constexpr std::string_view GET = "GET"sv;
  static constexpr std::string_view POST = "POST"sv;
  static constexpr std::string_view HEAD = "HEAD"sv;
};

struct Code {

  Code() = delete;

  static constexpr std::string_view BAD_REQUEST = "badRequest"sv;
  static constexpr std::string_view INVALID_METHOD = "invalidMethod"sv;
  static constexpr std::string_view INVALID_ARGUMENT = "invalidArgument"sv;
  static constexpr std::string_view INVALID_TOKEN = "invalidToken"sv;
  static constexpr std::string_view UNKNOWN_TOKEN = "unknownToken"sv;
  static constexpr std::string_view MAP_NOT_FOUND = "mapNotFound"sv;

};

struct ErrorInfo {
  ErrorInfo() = delete;
  static constexpr std::string_view CODE = "code"sv;
  static constexpr std::string_view MESSAGE = "message"sv;
};

static constexpr std::string_view NO_CACHE = "no-cache"sv;

struct Loot {

  Loot() = delete;

  static constexpr std::string_view LOOT_TYPES = "lootTypes"sv;
  static constexpr std::string_view LOOT_GEN_CONFIG = "lootGeneratorConfig"sv;
  static constexpr std::string_view PERIOD = "period"sv;
  static constexpr std::string_view PROBABILITY = "probability"sv;
  static constexpr std::string_view NAME = "name"sv;
  static constexpr std::string_view FILE = "file"sv;
  static constexpr std::string_view TYPE = "type"sv;
  static constexpr std::string_view ROTATION = "rotation"sv;
  static constexpr std::string_view COLOR = "color"sv;
  static constexpr std::string_view SCALE = "scale"sv;
  static constexpr std::string_view LOST_OBJECTS = "lostObjects"sv;
  static constexpr std::string_view VALUE = "value"sv;

};

struct DataBase {

  static constexpr size_t LIMIT_RECORDS = 100;
  static constexpr std::string_view DATADASE_URL = "GAME_DB_URL"sv;

};

struct Records {

  static constexpr std::string_view START = "start"sv;
  static constexpr std::string_view MAX_ITEMS = "maxItems"sv;
  static constexpr size_t START_DEFAULT= 0;
  static constexpr size_t MAX_ITEMS_DEFAULT = 100;
  static constexpr std::string_view NAME = "name"sv;
  static constexpr std::string_view SCORE = "score"sv;
  static constexpr std::string_view PLAY_TIME = "playTime"sv;

};

}