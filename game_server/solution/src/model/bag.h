#pragma once

#include <vector>
#include <stdexcept>
#include <string>
#include <numeric>

#include "lost_object.h"

namespace model {

  class Bag {
  
  public:

    explicit Bag(size_t capacity) : capacity_(capacity) {}

    bool Capacity() const {
    
      return lost_objects_.size() == capacity_;
      
    }

    bool Empty() const {
      
      return lost_objects_.empty();

    }

    void AddLostObject(LostObject lost_object) {
    
      if (Capacity()) {
        throw std::runtime_error(
          "Cannot add object. Bag is full. Current size: " +
          std::to_string(lost_objects_.size()) +
          ", capacity: " +
          std::to_string(capacity_));
      }
      lost_objects_.push_back(lost_object);

    }

    size_t ReturnObject() {

      size_t score = std::accumulate(
        lost_objects_.begin(), lost_objects_.end(), 0ull,
        [](size_t acc, const auto& object) {
          return acc + object.GetValue();
        }
      );

      lost_objects_.clear();
      return score;

    }

    size_t GetCapacity() const {
      return capacity_;
    }

    size_t Size() const {
      return lost_objects_.size();
    }

    std::vector<LostObject>::iterator begin() {
      return lost_objects_.begin();
    }

    std::vector<LostObject>::iterator end() {
      return lost_objects_.end();
    }

    std::vector<LostObject>::const_iterator begin() const {
      return lost_objects_.begin();
    }

    std::vector<LostObject>::const_iterator end() const {
      return lost_objects_.end();
    }


  private:

    size_t capacity_;
    std::vector<LostObject> lost_objects_;

  };

}