#pragma once

#include <random>

#include "../model/map.h"
#include "../model/dog.h"

namespace util {


 
  static model::Position GenerateRandomPosition(const model::Road& road, std::mt19937& gen) {
    if (road.IsHorizontal()) {
      std::uniform_real_distribution<double> distrib(road.GetStart().x, road.GetEnd().x);
      return model::Position{
          .x = distrib(gen),
          .y = static_cast<double>(road.GetStart().y)
      };
    }
    else {
      std::uniform_real_distribution<double> distrib(road.GetStart().y, road.GetEnd().y);
      return model::Position{
          .x = static_cast<double>(road.GetStart().x),
          .y = distrib(gen)
      };
    }
  }


  static size_t GenerateRandomRoadIndex(const model::Map::Roads& roads, std::mt19937& gen) {

    std::uniform_int_distribution<size_t> distrib(0, roads.size() - 1);
    return distrib(gen);

  }


  template<typename T>
  T GenerateRandomFromInterval(const T lower, const T upper) {
 
    std::random_device rd;
    std::default_random_engine eng(rd());

    if constexpr (std::is_integral_v<T>) {
      std::uniform_int_distribution<T> distr(lower, upper);
      return distr(eng);
    }
    else if constexpr (std::is_floating_point_v<T>) {
      std::uniform_real_distribution<T> distr(lower, upper);
      return distr(eng);
    }
  }


}