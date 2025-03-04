#pragma once

#include "position_repr.h"
#include "../lost_object.h"

namespace state_serialization {

  class LostObjectRepr {
  
  public:

    explicit LostObjectRepr() = default;

    explicit LostObjectRepr(const model::LostObject& lost_object) :
      id_(*lost_object.GetId()),
      position_(lost_object.GetPosition()),
      type_(lost_object.GetType()),
      selected_(lost_object.IsSelected()),
      value_(lost_object.GetValue())
    {}

    [[nodiscard]] model::LostObject ToLostObject() const {
      model::LostObject lost_object{
      model::LostObject::Id(id_),
      position_.ToPosition(),
      type_,
      value_,
      };
      if (selected_) {
        lost_object.SetSelected();
      }

      return lost_object;
    
    }

    template<typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
      ar& id_;
      ar& position_;
      ar& type_;
      ar& selected_;
      ar& value_;
      
    }
  
  private:

    size_t id_;
    PositionRepr position_;
    size_t type_;
    bool selected_ = false;
    size_t value_;
  };


}