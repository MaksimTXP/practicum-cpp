#include "game_session.h"


namespace model {

  std::shared_ptr<Dog> GameSession::CreateDog(const std::string& name, bool randomize_spawn_points) {

    auto dog = std::make_shared<Dog>(name, map_->GetBagCapacity());
    dogs_.push_back(dog);
    if (randomize_spawn_points) {
      SetRandomPositionOnMap(dog);
    }
    net::dispatch(*strand_, [self = shared_from_this()] {
      self->GenerateLoot(self->loot_gen_.GetTimeInterval());
      });

    return dog;
  }

  void GameSession::UpdateGameState(std::chrono::milliseconds delta_time) {
  
    auto roads = map_->GetRoads();

    for (auto& dog : dogs_) {
      dog->Move(delta_time, roads);

      dog->AddLiveTime(dog->GetLiveTime()+delta_time);
     
      if (dog->GetSpeed().x == 0 && dog->GetSpeed().y == 0) {
        dog->AddInactiveTime(dog->GetInactiveTime() + delta_time);
      }
      else {
        dog->AddInactiveTime(std::chrono::milliseconds(0));
      }
      if (dog->GetInactiveTime() >= max_inactive_time_) {
        continue;
      }
    }
    FindReturnLoot();
  }

  void GameSession::SetRandomPositionOnMap(std::shared_ptr<model::Dog> dog) {

    static std::random_device rd;
    static std::mt19937 gen(rd());
    const auto& roads = map_->GetRoads();
    size_t road_index = util::GenerateRandomRoadIndex(roads, gen);
    const auto& road = roads[road_index];
    model::Position position = util::GenerateRandomPosition(road, gen);
    dog->SetPosition(position);
  }

  model::LostObject GameSession::SetLostObject() {

    const auto& lost_types = map_->GetLootTypes();
    static std::random_device rd;
    static std::mt19937 gen(rd());
    const auto& roads = map_->GetRoads();
    size_t road_index = util::GenerateRandomRoadIndex(roads, gen);
    const auto& road = roads[road_index];
    auto point = util::GenerateRandomPosition(road, gen);
    size_t rand_number = util::GenerateRandomFromInterval(static_cast<size_t>(0), map_->GetLootTypes().size() - 1);
    return model::LostObject{ point, rand_number, static_cast<size_t>(lost_types[rand_number].value) };

  }

  void GameSession::GenerateLoot(const loot_gen::LootGenerator::TimeInterval& delta_time) {
    size_t generated_count =
      loot_gen_.Generate(delta_time, static_cast<unsigned>(lost_objects_.size()),
        static_cast<unsigned>(dogs_.size()));

    for (size_t i = 0; i < generated_count; ++i) {
      lost_objects_.push_back(SetLostObject());
    }
  }

  void GameSession::FindReturnLoot() {

    const auto& offices = map_->GetOffices();

    std::vector<collision_detector::Item> items;

    items.reserve(lost_objects_.size() + offices.size());

    for (const auto& object : lost_objects_) {
      items.emplace_back(object.GetPosition(), WIDTH_LOST_OBJECT);
    }

    size_t offices_start = items.size();

    for (const auto& office : offices) {

      Position position = { static_cast<double>(office.GetPosition().x) ,
        static_cast<double>(office.GetPosition().y) };

      items.emplace_back(position, WIDTH_OFFICE);
    }

    std::vector<collision_detector::Gatherer> gatherers;
    gatherers.reserve(dogs_.size());
    for (const auto& dog : dogs_) {
      gatherers.emplace_back(
        dog->GetPrevPosition(),
        dog->GetPosition(),
        WIDTH_DOG
      );
    }

    auto events = collision_detector::FindGatherEvents(ItemDogProvider(std::move(items), std::move(gatherers)));

    if (events.empty()) {

      return;
    }

    for (const auto& event : events) {


      Bag& bag = dogs_[event.gatherer_id]->GetBag();
      if (event.item_id < offices_start) {
        auto& object = lost_objects_[event.item_id];
        if (bag.Capacity() || object.IsSelected()) {

          continue;
        }
        object.SetSelected();
        bag.AddLostObject(object);
      }
      else {
        if (bag.Empty()) {
          continue;
        }
        auto& dog = dogs_[event.gatherer_id];
        dog->AddScore(dog->GetScore() + bag.ReturnObject());
      }
    }
    std::erase_if(lost_objects_, [](const auto& object) {
      return object.IsSelected();
      });

  }

}