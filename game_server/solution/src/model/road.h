#pragma once

#include "data_model.h"


namespace model {

class Road {

  struct HorizontalTag {
    HorizontalTag() = default;
  };

  struct VerticalTag {
    VerticalTag() = default;
  };

public:

  static constexpr HorizontalTag HORIZONTAL{};
  static constexpr VerticalTag VERTICAL{};


  Road(HorizontalTag, Point start, Coord end_x) noexcept
    : start_{ start }, end_{ end_x, start.y } {
  }


  Road(VerticalTag, Point start, Coord end_y) noexcept
    : start_{ start }, end_{ start.x, end_y } {
  }

  bool IsHorizontal() const noexcept {
    return start_.y == end_.y;
  }

  bool IsVertical() const noexcept {
    return start_.x == end_.x;
  }

  Point GetStart() const noexcept {
    return start_;
  }

  Point GetEnd() const noexcept {
    return end_;
  }


private:

  Point start_;
  Point end_;

};
}
