#pragma once

#include <memory>
#include <vector>

#include "map.h"
#include "game_session.h"
#include "../model/loot/loot_generator.h"



namespace model {

class Game {
public:

  Game(loot_gen::LootGenerator::TimeInterval time_interval, double probability, std::chrono::milliseconds max_inactive_time) : 
    loot_gen_(std::move(loot_gen::LootGenerator{time_interval, probability})),
    max_inactive_time_(std::move(max_inactive_time))
  {}

  using Maps = std::vector<std::shared_ptr<Map>>;

  void AddMap(Map map);

  void AddMaps(const std::vector<Map>& maps);

  const Maps& GetMaps() const noexcept;

  const std::shared_ptr<Map> FindMap(const Map::Id& id) const noexcept;

  void AddGameSession(std::shared_ptr<GameSession> session);

  std::shared_ptr<GameSession> FindGameSessionBy(const Map::Id& id) const noexcept;

  void UpdateGameState(std::chrono::milliseconds delta_time);

  const loot_gen::LootGenerator& GetLootGenerator() const {
    return loot_gen_;
  }

  loot_gen::LootGenerator& GetLootGenerator() {
    return loot_gen_;
  }

  const std::vector<std::shared_ptr<GameSession>> GetGameSession() const {
    return sessions_;
  }

  const std::chrono::milliseconds GetMaxInactiveTime() const {
    return max_inactive_time_;
  }

private:

  using MapIdHasher = util::TaggedHasher<Map::Id>;
  using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;

  Maps maps_;
  MapIdToIndex map_id_to_index_;
  std::vector<std::shared_ptr<GameSession>> sessions_;
  MapIdToIndex map_id_to_index_session_;
  loot_gen::LootGenerator loot_gen_;
  std::chrono::milliseconds max_inactive_time_;
};

}