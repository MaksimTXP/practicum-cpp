#pragma once

#include "dog_repr.h"
#include "lost_object_repr.h"
#include "../game_session.h"

#include <boost/serialization/vector.hpp>


namespace state_serialization{

  class GameSessionRepr {
  
  public:

    GameSessionRepr() = default;

    explicit GameSessionRepr(model::GameSession session):
    map_id_(*session.GetMap()->GetId())
    {
    const auto& dogs = session.GetDogs();
    dogs_.reserve(dogs.size());
    for (const auto dog : dogs) {
      dogs_.emplace_back(DogRepr(*dog));
    }
    const auto& lost_objects = session.GetLostObjects();
    lost_objects_.reserve(lost_objects.size());
    for (const auto& lost_object : lost_objects) {
      lost_objects_.emplace_back(LostObjectRepr(lost_object));
    }

    }

    [[nodiscard]] model::Map::Id ToMapId() const {
      return model::Map::Id(map_id_);
    }

    void ToDogs(model::GameSession& session) {
      for (const auto& dog : dogs_) {
        session.SetDog(std::make_shared<model::Dog>(dog.ToDog()));
      }
    
    }

    void ToLostObjects(model::GameSession& session) {
      for (const auto& lost_object : lost_objects_) {
        session.AddLostObject(lost_object.ToLostObject());
      }
    }

    template<typename Archive>

    void serialize(Archive& ar, [[maybe_unused]] const unsigned version)
    {
      ar& map_id_;
      ar& dogs_;
      ar& lost_objects_;
    }

  private:
    std::string map_id_;
    std::vector<DogRepr> dogs_;
    std::vector<LostObjectRepr> lost_objects_;
  };
  
}