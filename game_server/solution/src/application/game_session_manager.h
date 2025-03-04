#pragma once


#include "application.h"
#include <chrono>


namespace application{

namespace net = boost::asio;

class GameSessionManager {

public:


  using GameSessionIdToIndex = std::unordered_map<model::GameSession::Id,
    std::vector<std::shared_ptr<Player>>, util::TaggedHasher<model::GameSession::Id>>;

  GameSessionManager(model::Game game, net::io_context& io)
    : game_(std::move(game))
    , io_(io) {}

  const model::Game::Maps& GetMapList() const {
    return game_.GetMaps();
  }

  std::shared_ptr<model::Map> FindMap(const model::Map::Id& id) const noexcept {
    return game_.FindMap(id);
  }

  model::Game& GetGame() {
    return game_;
  }

  GameSessionIdToIndex& GetSessionId() {
    return session_id_to_players_;
  }

  void UpdateGameState(std::chrono::milliseconds delta_time) {
    game_.UpdateGameState(delta_time);
  }

  std::shared_ptr<model::GameSession> FindGameSessionBy(const model::Map::Id& id) const noexcept {
    return game_.FindGameSessionBy(id);
  }

  const std::vector<std::shared_ptr<model::GameSession>> GetGameSessions() const {
    return game_.GetGameSession();
  }

  net::io_context& GetContext() {
    return io_;
  }

  const std::chrono::milliseconds GetMaxInactiveTime() const {
  
    return game_.GetMaxInactiveTime();
  }

  void DeletePlayersGameSession(const std::chrono::milliseconds max_inactive_time) {
    for (auto& [_, player] : session_id_to_players_) {
      std::erase_if(player, [&](const auto& player) {
        return player->GetDog()->GetInactiveTime() >= max_inactive_time;
        });
    }
  
  }


private:

  model::Game game_;
  net::io_context& io_;
  GameSessionIdToIndex session_id_to_players_;
 
};

}  