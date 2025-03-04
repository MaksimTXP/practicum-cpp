#pragma once

#include "lost_object_repr.h"
#include "../bag.h"

#include <boost/serialization/vector.hpp>

#include <algorithm>

namespace state_serialization {

  class BagRepr {
  
  public:

    BagRepr() = default;

    explicit BagRepr(const model::Bag& bag) : capacity_(bag.GetCapacity()) {
      lost_objects_.reserve(bag.Size());
      for (const auto& lost_object : bag) {
        lost_objects_.emplace_back(LostObjectRepr(lost_object));
      }
    }

    [[nodiscard]] model::Bag ToBag() const {
      model::Bag bag(capacity_);
      for(const auto & lost_object: lost_objects_) {
        bag.AddLostObject(lost_object.ToLostObject());
      }
      return bag;
    }

    template<typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
      ar& capacity_;
      ar& lost_objects_;
    }


  private:
    size_t capacity_;
    std::vector<LostObjectRepr> lost_objects_;
  };

}