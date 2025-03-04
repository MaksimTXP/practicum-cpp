#pragma once

#include "../data_model.h"

namespace state_serialization {
  
  class SpeedDogRepr {

  public:
    SpeedDogRepr() = default;

    explicit SpeedDogRepr(const model::Speed& speed) : x_(speed.x), y_(speed.y) {}

    [[nodiscard]] model::Speed ToSpeedDogRepr() const {
      return model::Speed(x_, y_);
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
      ar& x_;
      ar& y_;
    }

  private:
    double x_;
    double y_;


  };

}