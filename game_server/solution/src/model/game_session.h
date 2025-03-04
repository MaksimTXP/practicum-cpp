#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

#include <memory>
#include <random>

#include "map.h"
#include "dog.h"
#include "item_dog_provider.h"
#include "../utils/tagged.h"
#include "../model/loot/loot_generator.h"
#include "lost_object.h"
#include "../http_server/ticker.h"
#include "../utils/random_generator.h"


namespace model {

namespace net = boost::asio;

class GameSession : public std::enable_shared_from_this<GameSession> {
public:

  using Strand = net::strand<net::io_context::executor_type>;
  using Id = util::Tagged<std::string, GameSession>;

  GameSession(std::shared_ptr<Map> map, net::io_context& ioc, loot_gen::LootGenerator& loot_gen,
    std::chrono::milliseconds max_inactive_time) :
    id_(*(map->GetId())),
    map_(map),
    strand_(std::make_shared<Strand>(net::make_strand(ioc))),
    loot_gen_(loot_gen),
    max_inactive_time_(max_inactive_time)
  {
    loot_ticker_ = std::make_shared<http_server::Ticker>(
      strand_,
      loot_gen_.GetTimeInterval(),
      [this](const auto& delta_time) {
        GenerateLoot(delta_time);
      }
    );
    loot_ticker_->Start();
  };

  const Id& GetId() const {
    return id_;
  }
  const std::shared_ptr<Map> GetMap() {
    return map_;
  }

  std::shared_ptr<Dog> CreateDog(const std::string& name, bool randomize_spawn_points);

  void SetDog(std::shared_ptr<Dog> dog) {
    dogs_.emplace_back(dog);
    net::dispatch(*strand_, [self = shared_from_this()] {
      self->GenerateLoot(self->loot_gen_.GetTimeInterval());
      });
  }

  std::shared_ptr<Strand> GetStrand() {
    return strand_;
  }

  void UpdateGameState(std::chrono::milliseconds delta_time);

  const std::vector<model::LostObject>& GetLostObjects() const {
    return lost_objects_;
  }

  model::LostObject SetLostObject();

  void GenerateLoot(const loot_gen::LootGenerator::TimeInterval& delta_time);
  
  const std::vector<std::shared_ptr<Dog>> GetDogs() const {
    return dogs_;
  }

  void AddLostObject(LostObject lost_object) {
    lost_objects_.emplace_back(lost_object);
  }

  void DeleteDog(std::shared_ptr<Dog> dog) {
    std::erase(dogs_, dog);
  }

private:

  void SetRandomPositionOnMap(std::shared_ptr<model::Dog> dog);

  void FindReturnLoot();

  Id id_;
  std::shared_ptr<Map> map_;
  std::vector<std::shared_ptr<Dog>> dogs_;
  std::shared_ptr<Strand> strand_;
  loot_gen::LootGenerator& loot_gen_;
  std::shared_ptr<http_server::Ticker> loot_ticker_;
  std::vector<model::LostObject> lost_objects_;
  std::chrono::milliseconds max_inactive_time_;

};
}