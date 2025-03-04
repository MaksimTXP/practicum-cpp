#pragma once

#include <algorithm>
#include <chrono>

#include "road_movement.h"
#include "data_model.h"
#include "map.h"
#include "bag.h"

namespace model {


class Dog {

  inline static size_t max_id_cont_ = 0;

  using Id = util::Tagged<size_t, Dog>;

public:


  Dog(const std::string& name, size_t capacity) :
    id_(Id{ Dog::max_id_cont_++ }),
    name_(name),
    bag_(capacity)
  {};

  Dog(Id id, const std::string& name, size_t capacity) :
    id_(id),
    name_(name),
    bag_(capacity)
  {};

  Dog(size_t id, std::string name, Speed speed, Position position,
    Position prev_position, Direction direction, Bag bag, size_t score) :
    id_(Id{ id }),
    name_(name),
    speed_(speed),
    position_(position),
    prev_position_(prev_position),
    direction_(direction),
    bag_(bag), 
    score_(score)
  {}

  const Id& GetId() const {
    return id_;
  }

  const std::string& GetName() const {
    return name_;
  }

  void SetSpeed(Speed speed) {
    speed_ = speed;
  }

  const Speed& GetSpeed() const {
    return speed_;
  }

  void SetPosition(const Position& position) {
    prev_position_ = position_;
    position_ = position;
  }

  const Position& GetPosition() const {
    return position_;
  }

  Position GetPrevPosition() const {
    return prev_position_;
  }

  void SetDirection(Direction direction) {
    direction_ = direction;
  }

  const Direction& GetDirection() const {
    return direction_;
  }

  void MoveDog(Direction direction, double speed);

  void Move(std::chrono::milliseconds delta_time, const Map::Roads& roads);

  Bag& GetBag() {
    
    return bag_;
  }

  const Bag& GetBag() const {

    return bag_;

  }

  void AddScore(size_t score) {
    score_ = score;
  }

  size_t GetScore() const {
    return score_;
  }

  void AddLiveTime(std::chrono::milliseconds live_time) {
    live_time_ = std::move(live_time);
  }

  const std::chrono::milliseconds GetLiveTime() const {
    return live_time_;
  }

  void AddInactiveTime(std::chrono::milliseconds inactive_time) {
    inactive_time_ = std::move(inactive_time);
  }

  const std::chrono::milliseconds GetInactiveTime() const {
    return inactive_time_;
  }


private:

  Id id_;
  std::string name_;
  Speed speed_{ 0.0, 0.0 };
  Position position_{ 0.0, 0.0 };
  Position prev_position_{ 0.0, 0.0 };
  Direction direction_{ Direction::NONE };
  RoadMovement road_movement_;
  Bag bag_;
  size_t score_ = 0;
  std::chrono::milliseconds live_time_{ 0 };
  std::chrono::milliseconds inactive_time_{ 0 };
};

}


