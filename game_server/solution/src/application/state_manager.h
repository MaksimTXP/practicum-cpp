#pragma once

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

#include "../model/state_serialization/player_repr.h"
#include "../model/game_session.h"
#include "player_manager.h"
#include "game_session_manager.h"
#include "player.h"
#include "player_tokens.h"



namespace application {

  class StateManager {
  public:

    StateManager(const std::string& state_file)
      : state_file_(state_file) {}

    void SaveState(const std::vector<std::shared_ptr<model::GameSession>>& game_sessions,
      const std::unordered_map<Token, std::shared_ptr<Player>, TokenHasher>& players) {
      if (state_file_.empty()) {
        return;
      }

      std::filesystem::create_directory(std::filesystem::path(state_file_).parent_path());

      const std::string out_file_name = state_file_ + ".tmp";

      std::ofstream out_put(out_file_name);
      if (!out_put) {
        throw std::runtime_error("State file not open");
      }


      boost::archive::text_oarchive oarchive{ out_put };

      std::vector<state_serialization::GameSessionRepr> game_sessions_repr;
      for (const auto& game_session : game_sessions) {
        game_sessions_repr.push_back(state_serialization::GameSessionRepr(*game_session));
      }
      oarchive << game_sessions_repr;

      std::vector<state_serialization::PlayerRepr> players_repr;
      for (const auto& [token, player] : players) {
        players_repr.push_back(state_serialization::PlayerRepr(*player, token));
      }
      oarchive << players_repr;

      out_put.close();

      std::filesystem::rename(out_file_name, state_file_);

    }

    void RestoreState(PlayerManager& player_manager, GameSessionManager& session_manager) {

      if (state_file_.empty()) {
        return;
      }

      std::ifstream in_put(state_file_);

      if (!in_put) {
        return;
      }

      boost::archive::text_iarchive iarchive{ in_put };

      std::vector<state_serialization::GameSessionRepr> game_sessions_repr;
      iarchive >> game_sessions_repr;
      std::vector<state_serialization::PlayerRepr> players_repr;
      iarchive >> players_repr;

      for (auto& game_session_repr : game_sessions_repr) {

        auto game_session = std::make_shared<model::GameSession>(session_manager.FindMap(game_session_repr.ToMapId()),
          session_manager.GetContext(), session_manager.GetGame().GetLootGenerator(), session_manager.GetMaxInactiveTime());
        session_manager.GetGame().AddGameSession(game_session);

        game_session_repr.ToLostObjects(*game_session);

      }

      for (auto& player_repr : players_repr) {
        auto player = std::make_shared<Player>(player_repr.ToPlayer());
        player_manager.AddPlayer(player, player_repr.ToToken());
        std::shared_ptr<model::GameSession> game_session = session_manager.FindGameSessionBy(player_repr.ToMapId());
        if (!game_session) {
          throw std::runtime_error("Session not found");
        }
        auto dog = std::make_shared<model::Dog>(player_repr.ToDog());
        player->SetGameSession(game_session);
        player->SetDog(dog);
        session_manager.GetSessionId()[game_session->GetId()].push_back(player);
      }

    }

  private:
    std::string state_file_;
  };

}