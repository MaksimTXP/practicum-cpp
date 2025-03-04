#pragma once

#include <string>
#include <optional>
#include <chrono>


namespace model {

using Dimension = int;
using Coord = Dimension;

struct Point {
  Coord x{ 0 };
  Coord y{ 0 };
};

struct Size {
  Dimension width{ 0 };
  Dimension height{ 0 };
};

struct Rectangle {
  Point position{ 0,0 };
  Size size{ 0,0 };
};

struct Offset {
  Dimension dx{ 0 };
  Dimension dy{ 0 };
};

struct Position {
  double x{ 0.0 };
  double y{ 0.0 };

};

struct Speed {
  double x{ 0.0 };
  double y{ 0.0 };
};

enum Direction {
  NORTH,
  SOUTH,
  WEST,
  EAST,
  NONE

};

struct InfoMovement {

  double distance;
  bool stop;

};

struct LootType {

  std::string name;
  std::string file;
  std::string type;
  std::optional<uint64_t> rotation;
  std::optional<std::string> color;
  double scale;
  int64_t value;

};

const double ROAD_WIDTH = 0.8;

static constexpr double DEFAULT_DOG_SPEED = 1.0;

static constexpr size_t DEFAULT_BAG_CAPACITY = 3;

static constexpr double MILLISECONDS_IN_SECOND = 1000.0;

static constexpr double WIDTH_OFFICE = 0.6;

static constexpr double WIDTH_DOG = 0.6;

static constexpr double WIDTH_LOST_OBJECT = 0.0;

static constexpr std::chrono::milliseconds DEFAULT_DOG_RETIREMENT_TIME{ 60000 };
}