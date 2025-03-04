#pragma once
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <sstream>

#include <optional>

#include "../application/application.h"
#include "../constants/config.h"
#include "../model/lost_object.h"
#include "../database/domain/player_record.h"
#include "utils.h"


namespace http_handler {

  namespace json = boost::json;
  namespace http = boost::beast::http;

  class ApiRequestHandler {
  public:

    using Players = std::vector<std::shared_ptr<application::Player>>;

    explicit ApiRequestHandler(application::Application& app) : app_(app) {}

    template<typename Body, typename Allocator, typename Send>
    void HandleApiRequest(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {

      boost::asio::dispatch(*app_.GetStrand(), [this, req = std::move(req), send = std::move(send)]() mutable {

        std::string_view target = req.target();
        size_t query_start = target.find('?');

        if (query_start != std::string_view::npos) {
          target = target.substr(0, query_start);
        }

        if (target == config::Endpoint::JOIN_GAME) {

          HandleGameJoinRequest(std::forward<http::request<http::string_body>>(req), send);

        }
        else if (target == config::Endpoint::PLAYERS_LIST) {

          HandlePlayersRequest(std::forward<http::request<http::string_body>>(req), send);

        }
        else if (target.starts_with(config::Endpoint::MAPS_LIST)) {

          HandleMapsRequest(std::forward<http::request<http::string_body>>(req), send);

        }
        else if (target == config::Endpoint::GAME_STATE) {

          HandleGameStateRequest(std::forward<http::request<http::string_body>>(req), send);

        }
        else if (target == config::Endpoint::ACTION) {

          HandlePlayerActionRequest(std::forward<http::request<http::string_body>>(req), send);

        }
        else if (target == config::Endpoint::TICK) {

          HandleGameTickRequest(std::forward<http::request<http::string_body>>(req), send);

        }
        else if (target == config::Endpoint::RECORDS) {
          HandleGameRecords(std::forward<http::request<http::string_body>>(req), send);
        }
        else {

          SendResponse(send, http::status::bad_request, config::Code::BAD_REQUEST,
            "Bad request", req);
        }
        });
    }

  private:

    application::Application& app_;

    template<typename Send>
    void SendResponse(Send&& send, http::status status, std::string_view code,
      std::string_view message, const http::request<http::string_body>& req,
      std::optional<std::string_view> allow_method = std::nullopt) {

      auto response = http_handler::MakeStringResponse(
        status,
        json::serialize(SerializeMessage(code, message)),
        req.version(), req.keep_alive(), config::ContentType::APPLICATION_JSON);
      response.set(http::field::cache_control, config::NO_CACHE);

      if (allow_method) {

        response.set(http::field::allow, *allow_method);

      }

      send(std::move(response));
    }

    template<typename Send>
    bool CheckRequestMethod(const http::request<http::string_body>& req,
      http::verb expected_method, Send&& send) {

      if (req.method() != expected_method) {

        std::string name_method;
        if (expected_method == http::verb::post) {
          name_method = config::RequestMethod::POST;
        }
        else if (expected_method == http::verb::get) {
          name_method = config::RequestMethod::GET;
        }
        SendResponse(send, http::status::method_not_allowed, config::Code::INVALID_METHOD,
          "Only " + name_method + " method is expected", req, name_method);
        return false;

      }
      return true;
    }

    template<typename Send>
    bool CheckRequestHeadMetod(const http::request<http::string_body>& req, Send&& send) {
      if (req.method() == http::verb::head) {

        auto response = http_handler::MakeStringResponse(
          http::status::ok,
          "",
          req.version(), req.keep_alive(), config::ContentType::APPLICATION_JSON);
        response.set(http::field::cache_control, config::NO_CACHE);
        send(std::move(response));
        return false;
      }
      return true;
    }

    template<typename Send>
    bool IsNotRequestGet(const http::request<http::string_body>& req, Send&& send) {

      if (req.method() != http::verb::get) {

        SendResponse(send, http::status::method_not_allowed, config::Code::INVALID_METHOD,
          "Invalid method", req,
          std::string(config::RequestMethod::GET) + ", " + std::string(config::RequestMethod::HEAD));

        return false;
      }
      return true;
    }

    template<typename Send>
    bool CheckRequestGetHeadMetod(const http::request<http::string_body>& req, Send&& send) {
      if (req.method() != http::verb::get && req.method() != http::verb::head) {

        SendResponse(send, http::status::method_not_allowed, config::Code::INVALID_METHOD,
          "Method Not Allowed", req, std::string(config::RequestMethod::GET) + ", " + std::string(config::RequestMethod::HEAD));
        return false;

      }
      return true;
    }

    template<typename Send>
    bool CheckContentType(const http::request<http::string_body>& req, Send&& send) {
      if (req[http::field::content_type] != config::ContentType::APPLICATION_JSON) {
        SendResponse(send, http::status::bad_request, config::Code::INVALID_ARGUMENT,
          "Invalid Content-Type", req);
        return false;
      }
      return true;
    }

    template<typename Send>
    bool ValidateAuthorizationHeader(const http::request<http::string_body>& req,
      Send&& send) {

      auto auth_header = req.find(http::field::authorization);

      if (auth_header == req.end()) {
        SendResponse(send, http::status::unauthorized, config::Code::INVALID_TOKEN,
          "Authorization header is required", req);
        return false;
      }

      std::string auth_value = std::string(auth_header->value());
      if (auth_value.size() <= 7 || auth_value.substr(0, 7) != "Bearer ") {

        SendResponse(send, http::status::unauthorized, config::Code::INVALID_TOKEN,
          "Authorization header is invalid or token is missing", req);

        return false;
      }

      std::string token_str = auth_value.substr(7);


      if (token_str.length() != 32) {

        SendResponse(send, http::status::unauthorized, config::Code::INVALID_TOKEN,
          "Token must be 16 characters long", req);

        return false;
      }


      if (std::any_of(token_str.begin(), token_str.end(), [](unsigned char c) { return std::isupper(c); })) {

        SendResponse(send, http::status::unauthorized, config::Code::UNKNOWN_TOKEN,
          "Token must not contain uppercase letters", req);

        return false;
      }


      if (!std::all_of(token_str.begin(), token_str.end(), [](unsigned char c) {
        return std::isdigit(c) || (c >= 'a' && c <= 'f');
        })) {
        SendResponse(send, http::status::unauthorized, config::Code::INVALID_TOKEN,
          "Token must contain only hex digits", req);
        return false;
      }

      application::Token token{ token_str };


      if (!app_.HasPlayer(token)) {
        SendResponse(send, http::status::unauthorized, config::Code::UNKNOWN_TOKEN,
          "Player token has not been found", req);

        return false;
      }

      return true;
    }


    template<typename Send>
    void HandleGameJoinRequest(const http::request<http::string_body>& req, Send&& send) {

      if (!CheckRequestMethod(req, http::verb::post, send) || !CheckContentType(req, send)) {
        return;
      }

      boost::system::error_code ec;
      json::value body = json::parse(req.body(), ec);

      if (ec) {

        SendResponse(send, http::status::bad_request, config::Code::INVALID_ARGUMENT,
          "Join game request parse error", req);
        return;
      }

      const auto& obj = body.as_object();

      if (!obj.contains(std::string(config::MapKey::USER_NAME)) || !obj.contains(std::string(config::MapKey::MAP_ID))) {

        SendResponse(send, http::status::bad_request, config::Code::INVALID_ARGUMENT,
          "Missing userName or mapId", req);
        return;
      }

      std::string user_name = obj.at(std::string(config::MapKey::USER_NAME)).as_string().c_str();
      std::string map_id = obj.at(std::string(config::MapKey::MAP_ID)).as_string().c_str();

      if (user_name.empty()) {

        SendResponse(send, http::status::bad_request, config::Code::INVALID_ARGUMENT,
          "Invalid name", req);
        return;
      }

      const auto map = app_.FindMap(model::Map::Id{ map_id });

      if (!map) {

        SendResponse(send, http::status::not_found, config::Code::MAP_NOT_FOUND,
          "Map not found", req);
        return;
      }

      auto join_result = app_.JoinGame(model::Map::Id{ map_id }, user_name);

      send(std::move(CreateJsonResponse(req, http::status::ok,
        SerializePlayer(join_result))));
    }

    template<typename Send>
    void HandlePlayersRequest(const http::request<http::string_body>& req, Send&& send) {

      if (!CheckRequestHeadMetod(req, send)) {
        return;
      }

      if (!IsNotRequestGet(req, send)) {
        return;
      }

      if (!ValidateAuthorizationHeader(req, send)) {
        return;

      }

      application::Token token = GetToken(req);

      const auto& players = app_.GetPlayersFromGameSession(token);

      send(std::move(CreateJsonResponse(req, http::status::ok, SerializePlayers(players))));
    }

    template<typename Send>
    void HandleMapsRequest(const http::request<http::string_body>& req, Send&& send) {

      if (!CheckRequestGetHeadMetod(req, send)) {
        return;

      }

      std::string_view target = req.target();

      if (target == config::Endpoint::MAPS_LIST) {

        send(std::move(CreateJsonResponse(req, http::status::ok, SerializeMapsList())));

      }

      else if (target.starts_with(config::Endpoint::MAPS_LIST) && !target.ends_with("/")) {

        std::string_view id = target.substr(config::Endpoint::MAPS_LIST.size() + 1);
        json::value map_info = SerializeMapInfo(id);

        if (map_info.is_object() && map_info.as_object().contains(std::string(config::ErrorInfo::CODE)) &&
          map_info.as_object()[std::string(config::ErrorInfo::CODE)].as_string() == config::Code::MAP_NOT_FOUND) {

          send(std::move(CreateJsonResponse(req, http::status::not_found, map_info)));
        }
        else {

          send(std::move(CreateJsonResponse(req, http::status::ok, map_info)));
        }
      }

      else {
        SendResponse(send, http::status::bad_request, config::Code::BAD_REQUEST, "Bad request", req);
      }
    }

    template<typename Send>
    void HandleGameStateRequest(const http::request<http::string_body>& req, Send&& send) {

      if (!CheckRequestHeadMetod(req, send)) {

        return;
      }

      if (!IsNotRequestGet(req, send)) {
        return;
      }

      if (!ValidateAuthorizationHeader(req, send)) {
        return;
      }

      application::Token token = GetToken(req);

      const auto& players = app_.GetPlayersFromGameSession(token);

      const auto& lost_objects = app_.GetLostObjects(token);

      send(std::move(CreateJsonResponse(req, http::status::ok, SerializePlayersInfoAndLostObject(players, lost_objects))));

    }

    template<typename Send>
    void HandlePlayerActionRequest(const http::request<http::string_body>& req, Send&& send) {

      if (!CheckRequestMethod(req, http::verb::post, send) || !CheckContentType(req, send)) {
        return;
      }

      if (!ValidateAuthorizationHeader(req, send)) {
        return;
      }

      application::Token token = GetToken(req);


      boost::json::value body;

      try {
        body = boost::json::parse(req.body());
      }
      catch (const std::exception&) {

        SendResponse(send, http::status::bad_request, config::Code::INVALID_ARGUMENT,
          "Failed to parse action", req);

        return;
      }

      if (!body.is_object() || !body.as_object().contains(std::string(config::MapKey::MOVE)) ||
        !body.at(std::string(config::MapKey::MOVE)).is_string()) {

        SendResponse(send, http::status::bad_request, config::Code::INVALID_ARGUMENT,
          "Invalid move parameter", req);

        return;
      }

      std::string move_value = body.at(std::string(config::MapKey::MOVE)).as_string().c_str();

      Move(move_value, token);

      send(std::move(CreateJsonResponse(req, http::status::ok, boost::json::parse("{}"))));


    }

    template<typename Send>
    void HandleGameTickRequest(const http::request<http::string_body>& req, Send&& send) {

      if (!CheckRequestMethod(req, http::verb::post, send) || !CheckContentType(req, send)) {
        return;
      }

      boost::json::value body;
      try {
        body = boost::json::parse(req.body());
      }
      catch (const std::exception&) {

        SendResponse(send, http::status::bad_request, config::Code::INVALID_ARGUMENT,
          "Failed to parse tick request JSON", req);

        return;
      }

      if (!body.is_object() || !body.as_object().contains(std::string(config::MapKey::TIME_DELTA))) {

        SendResponse(send, http::status::bad_request, config::Code::INVALID_ARGUMENT,
          "Invalid or missing timeDelta parameter", req);

        return;
      }

      try {
        int64_t tempTimeDelta = body.at(std::string(config::MapKey::TIME_DELTA)).as_int64();
        if (tempTimeDelta < 0) {
          throw std::invalid_argument("Negative timeDelta is not allowed");
        }
        std::chrono::milliseconds timeDelta = std::chrono::milliseconds(tempTimeDelta);

        app_.UpdateGameState(timeDelta);

        send(std::move(CreateJsonResponse(req, http::status::ok, boost::json::parse("{}"))));
      }
      catch (...) {

        SendResponse(send, http::status::bad_request, config::Code::INVALID_ARGUMENT,
          "timeDelta is not a valid uint64", req);

      }
    }

    template<typename Send>
    void HandleGameRecords(const http::request<http::string_body>& req, Send&& send) {

      if (!IsNotRequestGet(req, send)) {
        return;
      }


      std::map<std::string, std::string> params;

      std::string target = std::string(req.target());

      size_t query_start = target.find('?');

      if (query_start != std::string::npos) {
        std::string query = target.substr(query_start + 1);
        std::istringstream iss(query);
        std::string pair;
        while (std::getline(iss, pair, '&')) {
          size_t equal_pos = pair.find('=');
          if (equal_pos != std::string::npos) {
            std::string key = pair.substr(0, equal_pos);
            std::string value = pair.substr(equal_pos + 1);
            params[key] = value;
          }
        }
      }

      size_t start = config::Records::START_DEFAULT;
      size_t max_items = config::Records::MAX_ITEMS_DEFAULT;

      if (params.find(std::string(config::Records::START)) != params.end()) {
        start = std::stoi(params[std::string(config::Records::START)]);
      }
      if (params.find(std::string(config::Records::MAX_ITEMS)) != params.end()) {
        max_items = std::stoi(params[std::string(config::Records::MAX_ITEMS)]);
      }

      const auto& records = app_.GetPlayersRecord(start, max_items);

      send(std::move(CreateJsonResponse(req, http::status::ok, SerializePlayersRecords(records))));

    }

    StringResponse CreateJsonResponse(const http::request<http::string_body>& req, http::status status,
      const json::value& value);

    application::Token GetToken(const http::request<http::string_body>& req);
    void Move(std::string move_value, application::Token token);

    json::value SerializeMapsList() const;

    json::value SerializeMapInfo(std::string_view id);

    json::value SerializeMessage(std::string_view code, std::string_view message);

    json::value SerializePlayer(application::JoinGameData join_result);

    json::value SerializePlayers(const Players& players);

    json::value SerializePlayersInfoAndLostObject(const Players& players, const std::vector<model::LostObject>& lost_objects);

    json::value SerializePlayersRecords(const std::vector<db_domain::PlayerRecord>& records);

  };
}
