#pragma once

#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>



#include <filesystem>
#include <fstream>

#include <random>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

#include "../model/map.h"
#include "../model/game.h"
#include "../model/game_session.h"
#include "../http_server/ticker.h"
#include "../database/postgres/postgres.h"
#include "../database/app/use_cases_impl.h"
#include "state_manager.h"
#include "player.h"
#include "player_tokens.h"
#include "player_manager.h"
#include "game_session_manager.h"


namespace application {

  namespace net = boost::asio;

  struct JoinGameData {
    Token token;
    Player::Id id;
  };

  class Application {
  public:
    using Strand = net::strand<net::io_context::executor_type>;

    Application(const model::Game& game, net::io_context& io,
      const std::chrono::milliseconds& tick_period,
      bool randomize_spawn_points, std::string state_file, std::chrono::milliseconds save_state_period, 
      size_t size_pool = 1, std::string url = "GAME_DB_URL")
      : session_manager_({ game, io })
      , randomize_spawn_points_(randomize_spawn_points)
      , strand_(std::make_shared<Strand>(net::make_strand(io)))
      , state_manager_(state_file)
      , save_state_period_(save_state_period)
      , data_base_(size_pool, url)
    {
      state_manager_.RestoreState(player_manager_, session_manager_);
      if (tick_period.count() != 0) {
        ticker_ = std::make_shared<http_server::Ticker>(strand_, tick_period,
          [&](std::chrono::milliseconds delta) {
            UpdateGameState(delta);
          });
        ticker_->Start();
      }
    }


    const model::Game::Maps& GetMapList() const {
      return session_manager_.GetMapList();
    }

    std::shared_ptr<model::Map> FindMap(const model::Map::Id& id) const noexcept {
      return session_manager_.FindMap(id);
    }

    JoinGameData JoinGame(const model::Map::Id& map_id, const std::string& name);

    const std::vector<std::shared_ptr<Player>>& GetPlayersFromGameSession(const Token& token);

    void MovePlayer(const Token& token, model::Direction direction);

    std::shared_ptr<Strand> GetStrand() const {
      return strand_;
    }

    bool HasPlayer(const Token& token) const {
      return player_manager_.HasPlayer(token);
    }

    void UpdateGameState(std::chrono::milliseconds delta_time);

    const std::vector<model::LostObject>& GetLostObjects(const Token& token);


    void SaveStateGame() {

      state_manager_.SaveState(session_manager_.GetGameSessions(), player_manager_.GetPayers());
    }

    std::vector<db_domain::PlayerRecord> GetPlayersRecord(const size_t start, const size_t max_items) {
    
      return use_cases_impl_.GetPlayersRecords(start, max_items);

     }

  private:

    void DisablingInactiveones() {

      std::vector<db_domain::PlayerRecord> records;

      auto player_record = player_manager_.DeletePlayers(session_manager_.GetMaxInactiveTime());
      session_manager_.DeletePlayersGameSession(session_manager_.GetMaxInactiveTime());
      for (const auto& player : player_record) {
        const auto& dog = player->GetDog();
        records.emplace_back(db_domain::PlayerRecord{ player->GetName() , dog->GetScore(), dog->GetLiveTime() });
        player->GetGameSession()->DeleteDog(dog);
      }
      use_cases_impl_.SavePlayersRecords(records);

    }

    PlayerManager player_manager_;
    GameSessionManager session_manager_;
    static const std::vector<std::shared_ptr<Player>> empty_player_list_;
    static const std::vector<model::LostObject> empty_lost_object_;
    bool randomize_spawn_points_;
    std::shared_ptr<Strand> strand_;
    std::shared_ptr<http_server::Ticker> ticker_;
    StateManager state_manager_;
    std::chrono::milliseconds save_state_period_{ 0 };
    std::chrono::milliseconds time_without_saving_{ 0 };
    db_postgres::Database data_base_;
    db_app::UseCasesImpl use_cases_impl_{ data_base_.GetUnitOfWorkFactory() };
    
  };

}
