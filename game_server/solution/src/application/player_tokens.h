#pragma once

#include <random>
#include <sstream>
#include <iomanip>

#include "player.h"

namespace application {

struct TokenTag {};

using Token = util::Tagged<std::string, TokenTag>;
using TokenHasher = util::TaggedHasher<Token>;

class PlayerTokens {
public:

  PlayerTokens() = default;
  PlayerTokens(const PlayerTokens&) = default;
  PlayerTokens& operator = (const PlayerTokens&) = default;

  Token AddPlayer(std::shared_ptr<Player> player) {
    Token token = GenerateToken();
    token_players_[token] = player;
    return token;
  }

  void AddPlayer(std::shared_ptr<Player> player, Token token) {
    token_players_[token] = player;
  }

  std::shared_ptr<Player> FindPlayerBy(Token token) const {
    auto it = token_players_.find(token);
    return it != token_players_.end() ? it->second : std::shared_ptr<Player>();
  }

  const std::unordered_map< Token, std::shared_ptr<Player>, TokenHasher >& GetTokensPlayer() const {
    return token_players_;
  }

  void DeleteToken(const std::chrono::milliseconds& max_inactive_time) {
    
    std::erase_if(token_players_, [&](const std::pair<Token, std::shared_ptr<Player>>& player_by_token) {
      const auto& player = player_by_token.second;
      return player->GetDog()->GetInactiveTime() >= max_inactive_time;
      });
   
  
  }

private:

  Token GenerateToken() {
    std::stringstream ss;
    const size_t token_size = 16;
    const auto add_hex_number = [&](auto x) {
      ss << std::setfill('0') << std::setw(token_size) << std::hex << x;
      };
    add_hex_number(generator1_());
    add_hex_number(generator2_());

    return Token{ ss.str() };
  }

  std::unordered_map< Token, std::shared_ptr<Player>, TokenHasher > token_players_;
  std::random_device random_device_;
  std::mt19937_64 generator1_{ [this] {
    std::uniform_int_distribution<std::mt19937_64::result_type> dist;
    return dist(random_device_);
  }() };
  std::mt19937_64 generator2_{ [this] {
    std::uniform_int_distribution<std::mt19937_64::result_type> dist;
    return dist(random_device_);
  }() };

};
}