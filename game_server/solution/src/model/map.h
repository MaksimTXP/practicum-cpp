#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

#include "road.h"
#include "building.h"
#include "office.h"


namespace model {

class Map {

public:
  using Id = util::Tagged<std::string, Map>;
  using Roads = std::vector<Road>;
  using Buildings = std::vector<Building>;
  using Offices = std::vector<Office>;

  Map(Id id, const std::string& name, double dog_speed, size_t bag_capacity) noexcept
    : id_(std::move(id))
    , name_(name)
    , dog_speed_(dog_speed)
    , bag_capacity_(bag_capacity)
  {
  }

  const Id& GetId() const noexcept {
    return id_;
  }

  const std::string& GetName() const noexcept {
    return name_;
  }

  const Buildings& GetBuildings() const noexcept {
    return buildings_;
  }

  const Roads& GetRoads() const noexcept {
    return roads_;
  }

  const Offices& GetOffices() const noexcept {
    return offices_;
  }

  void AddRoad(const Road& road) {
    roads_.emplace_back(road);
  }

  void AddBuilding(const Building& building) {
    buildings_.emplace_back(building);
  }

  void AddOffice(Office office);

  void SetDogSpeed(double speed) {
    dog_speed_ = std::abs(speed);
  }

  double GetDogSpeed() const noexcept {
    return dog_speed_;
  }

  size_t GetBagCapacity() const {

    return bag_capacity_;

  }

  const std::vector<model::LootType>& GetLootTypes() const noexcept {
    return loot_types_;
  }

  void AddLootType(const model::LootType& loot_type) {
    loot_types_.emplace_back(loot_type);
  }



private:


  using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

  Id id_;
  std::string name_;
  Roads roads_;
  Buildings buildings_;
  std::vector<model::LootType> loot_types_;
  OfficeIdToIndex warehouse_id_to_index_;
  Offices offices_;
  double dog_speed_;
  size_t bag_capacity_;
};
}