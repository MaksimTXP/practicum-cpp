#pragma once
#include "../data_model.h"

namespace state_serialization {

  class PositionRepr {

  public:

    explicit PositionRepr() = default;

    explicit PositionRepr(const model::Position& pos) : x_(pos.x), y_(pos.y) {}

    [[nodiscard]] model::Position ToPosition() const {
      return { x_, y_ };
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version ) {
      ar& x_;
      ar& y_;
    }

  private:

    double x_;
    double y_;

  };

}