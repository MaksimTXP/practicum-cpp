#pragma once

#include "bag_repr.h"
#include "position_repr.h"
#include "speed_dog_repr.h"
#include "../dog.h"


namespace state_serialization {

  class DogRepr {
  public:

    DogRepr() = default;

    explicit DogRepr(const model::Dog& dog) :
      id_(*dog.GetId()),
      name_(dog.GetName()),
      speed_(dog.GetSpeed()),
      position_(dog.GetPosition()),
      prev_position_(dog.GetPrevPosition()),
      direction_(static_cast<int>(dog.GetDirection())),
      bag_(dog.GetBag()),
      score_(dog.GetScore())
    {}

    [[nodiscard]] model::Dog ToDog() const {
      return model::Dog{
      id_,
      name_,
      speed_.ToSpeedDogRepr(),
      position_.ToPosition(),
      prev_position_.ToPosition(),
      static_cast<model::Direction>(direction_),
      bag_.ToBag(),
      score_,
      };
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
      ar& id_;
      ar& name_;
      ar& speed_;
      ar& position_;
      ar& prev_position_;
      ar& direction_;
      ar& bag_;
      ar& score_;
    }


  private:

    size_t id_;
    std::string name_;
    SpeedDogRepr speed_;
    PositionRepr position_;
    PositionRepr prev_position_;
    int direction_;
    BagRepr bag_;
    size_t score_;


  };

}

