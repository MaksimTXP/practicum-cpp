#include "application.h"

namespace application {

const std::vector<std::shared_ptr<Player>> Application::empty_player_list_{};
const std::vector<model::LostObject> Application::empty_lost_object_{};

JoinGameData Application::JoinGame(const model::Map::Id& map_id, const std::string& name) {

  auto player = player_manager_.CreatePlayer(name);
  auto token = player_manager_.AddPlayer(player);

  auto game_session = session_manager_.GetGame().FindGameSessionBy(map_id);

  if (!game_session) {

    game_session = std::make_shared<model::GameSession>(session_manager_.FindMap(map_id),
      session_manager_.GetContext(), session_manager_.GetGame().GetLootGenerator(), session_manager_.GetGame().GetMaxInactiveTime());
    session_manager_.GetGame().AddGameSession(game_session);

  }
  session_manager_.GetSessionId()[game_session->GetId()].push_back(player);
  player->SetGameSession(game_session);
  player->SetDog(game_session->CreateDog(player->GetName(), randomize_spawn_points_));

  return { token, player->GetId() };
}

const std::vector<std::shared_ptr<Player>>& Application::GetPlayersFromGameSession(const Token& token) {

  auto player = player_manager_.FindPlayerByToken(token);
  if (!player) {
    return empty_player_list_;
  }

  auto session_id = player->GetGameSessionId();

  if (!session_manager_.GetSessionId().contains(session_id)) {
    return empty_player_list_;
  }

  return session_manager_.GetSessionId()[session_id];
}


void Application::MovePlayer(const Token& token, model::Direction direction) {

  auto player = player_manager_.FindPlayerByToken(token);
  auto dog = player->GetDog();
  double speed = player->GetGameSession()->GetMap()->GetDogSpeed();
  dog->MoveDog(direction, speed);
  if (direction == model::Direction::NONE) {
    dog->SetDirection(direction);
  }

}

const std::vector<model::LostObject>& Application::GetLostObjects(const Token& token) {

  auto player = player_manager_.FindPlayerByToken(token);

  if (!player) {
    throw std::invalid_argument("The player is missing");
  }

  auto session = player->GetGameSession();
  if (!session || !session_manager_.GetSessionId().contains(session->GetId())) {
    return empty_lost_object_;
  }

  return session->GetLostObjects();

}

void Application::UpdateGameState(std::chrono::milliseconds delta_time) {
  
  session_manager_.UpdateGameState(delta_time);

  DisablingInactiveones();

  time_without_saving_ += delta_time;
  if (time_without_saving_ >= save_state_period_) {
    time_without_saving_ = std::chrono::milliseconds(0);
    SaveStateGame();
  }
}


}