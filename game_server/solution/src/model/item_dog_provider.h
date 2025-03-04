#pragma once 

#include "collision_detector/collision_detector.h"


namespace model {

  class ItemDogProvider : public collision_detector::ItemGathererProvider {
  
  public:

    ItemDogProvider(const std::vector<collision_detector::Item>& items, 
      const std::vector<collision_detector::Gatherer>& gatherers) :
      items_(items),
      gatherers_(gatherers) {}

    size_t ItemsCount() const override {
      return items_.size();
    }

    size_t GatherersCount() const override {
      return gatherers_.size();
    }

    collision_detector::Item GetItem(size_t index) const override {
      return items_[index];
    }

    collision_detector::Gatherer GetGatherer(size_t index) const override {
      return gatherers_[index];
    }


  private:
  
    std::vector<collision_detector::Item> items_;
    std::vector<collision_detector::Gatherer> gatherers_;


  };

}