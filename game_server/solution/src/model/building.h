#pragma once


#include "data_model.h"

namespace model {

class Building {
public:
  explicit Building(const Rectangle& bounds) noexcept
    : bounds_{ bounds } {
  }

  const Rectangle& GetBounds() const noexcept {
    return bounds_;
  }

private:
  Rectangle bounds_;
};
}