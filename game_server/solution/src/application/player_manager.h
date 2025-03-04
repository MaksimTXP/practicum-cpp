#pragma once

#include "../application/application.h"

namespace application
{

  class PlayerManager
  {
  public:
    std::shared_ptr<Player> CreatePlayer(const std::string &name)
    {

      auto player = std::make_shared<Player>(name);
      players_.push_back(player);

      return player;
    }

    Token AddPlayer(const std::shared_ptr<Player> &player)
    {
      return player_tokens_.AddPlayer(player);
    }

    void AddPlayer(const std::shared_ptr<Player> &player, const Token &token)
    {
      player_tokens_.AddPlayer(player, token);
    }

    std::shared_ptr<Player> FindPlayerByToken(const Token &token) const
    {
      return player_tokens_.FindPlayerBy(token);
    }

    bool HasPlayer(const Token &token) const
    {
      return FindPlayerByToken(token) != nullptr;
    }

    const std::unordered_map<Token, std::shared_ptr<Player>, TokenHasher> GetPayers() const
    {

      return player_tokens_.GetTokensPlayer();
    }

    std::vector<std::shared_ptr<Player>> DeletePlayers(const std::chrono::milliseconds &max_inactive_time)
    {

      std::vector<std::shared_ptr<Player>> delete_players;

      std::erase_if(players_, [&](const std::shared_ptr<Player> &player){
      if (player->GetDog()->GetInactiveTime() >= max_inactive_time) {

        delete_players.emplace_back(player);
       
        return true;
      }

      return false; });

      player_tokens_.DeleteToken(max_inactive_time);
      return delete_players;
    }

  private:
    std::vector<std::shared_ptr<Player>> players_;

    PlayerTokens player_tokens_;

    static const std::vector<std::shared_ptr<Player>> empty_players_list_;
  };
}