#pragma once

#include "../utils/tagged.h"
#include "data_model.h"

namespace model {

  class LostObject {

  public:
    using Id = util::Tagged<size_t, LostObject>;


    LostObject(Id id, Position position, size_t type, size_t value) noexcept :
      id_(id),
      position_{ position },
      type_(type),
      value_(value)
    {}

    LostObject(Position position, size_t type, size_t value) noexcept :
      id_{Id(free_id_++)},
      position_{ position },
      type_(type),
      value_(value)
    {}

    const Id& GetId() const noexcept {
      return id_;
    }

    Position GetPosition() const noexcept {
      return position_;
    }

    size_t GetType() const noexcept {
      return type_;
    }

    void SetSelected() {
      selected_ = true;
    }

    bool IsSelected() const {
      return selected_;
    }

    size_t GetValue() const {
      return value_;
    }

  private:
    
    inline static size_t free_id_ = 0;

    Id id_;
    Position position_;
    size_t type_;
    bool selected_ = false;
    size_t value_;

  };

}