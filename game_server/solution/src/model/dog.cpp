#include "dog.h"
#include <chrono>

namespace model {

void Dog::MoveDog(Direction direction, double speed) {

  switch (direction) {
  case Direction::NORTH: {
    SetDirection(direction);
    SetSpeed({ 0, -speed });
    break;
  }
  case Direction::SOUTH: {
    SetDirection(direction);
    SetSpeed({ 0, speed });
    break;
  }
  case Direction::WEST: {
    SetDirection(direction);
    SetSpeed({ -speed, 0 });
    break;
  }
  case Direction::EAST: {
    SetDirection(direction);
    SetSpeed({ speed, 0 });
    break;
  }
  case Direction::NONE: {
    SetSpeed({ 0, 0 });
    break;
  }
  }
}

void Dog::Move(std::chrono::milliseconds delta_time, const Map::Roads& roads) {
  
  if (speed_.x == 0.0 && speed_.y == 0.0) {
    if (direction_ != Direction::NONE) {
      inactive_time_ = std::chrono::milliseconds(0);
    }
    return;
  }
  double time = delta_time.count() / MILLISECONDS_IN_SECOND;
  std::vector<InfoMovement> movement_results;

  for (const auto& road : roads) {
    movement_results.push_back(road_movement_.MoveOnRoad(time, road, direction_, speed_, position_));
  }

  std::sort(movement_results.begin(), movement_results.end(), [](const InfoMovement& first,
    const InfoMovement& second) {
      if (first.distance < second.distance) {
        return true;
      }
      return static_cast<int>(first.stop) > static_cast<int>(second.stop);
    });

  InfoMovement applied_movement = movement_results.back();

  position_.x += (speed_.x > 0 ? 1 : (speed_.x < 0 ? -1 : 0)) * applied_movement.distance;

  if (position_.x < -ROAD_WIDTH / 2) {
    position_.x = -ROAD_WIDTH / 2;

  }

  position_.y += (speed_.y > 0 ? 1 : (speed_.y < 0 ? -1 : 0)) * applied_movement.distance;

  if (applied_movement.stop) {
    speed_ = { 0.0, 0.0 };
    inactive_time_ += delta_time;
    if (direction_ != Direction::NONE) {
      inactive_time_ = std::chrono::milliseconds(0); 
    }
  }
}

}