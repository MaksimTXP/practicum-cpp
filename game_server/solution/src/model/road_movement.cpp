#include "road_movement.h"


namespace model {


InfoMovement RoadMovement::MoveOnRoad(double time, const Road& road,
  Direction& direction, Speed& speed, Position& position) {

  const double EPS = 0.000001;

  if (road.IsVertical()) {

    return MoveOnVerticalRoad(time, road, direction, speed, position);
  }

  else {
    return  MoveOnHorizontalRoad(time, road, direction, speed, position);
  }
  return { 0.0, true };
}


InfoMovement RoadMovement::MoveOnVerticalRoad(double time, const Road& road,
  Direction& direction, Speed& speed, Position& position) {
  bool is_stop_required = false;

  if (!IsWithinXVerticaBounds(position.x, road)) {
    return { 0.0, true };
  }

  if (!IsWithinYVerticaBounds(position.y, road)) {
    return { 0.0, true };
  }

  switch (direction) {
  case NORTH:
    return { MoveNorthOnVerticalRoad(position.y, road, speed.y,
      time, is_stop_required), is_stop_required };
  case SOUTH:
    return { MoveSouthOnVerticalRoad(position.y, road, speed.y,
      time, is_stop_required), is_stop_required };
  case WEST:
    return { MoveWestOnVerticalRoad(position.x, road, speed.x,
      time, is_stop_required), is_stop_required };
  case EAST:
    return { MoveEastOnVerticalRoad(position.x, road, speed.x,
      time, is_stop_required), is_stop_required };
  }

  return { 0.0, true };
}

 bool RoadMovement::IsWithinXVerticaBounds(double current_x, const Road& road) {
  double left = static_cast<double>(road.GetStart().x - ROAD_WIDTH / 2);
  if (left < -ROAD_WIDTH / 2) {
    left = -ROAD_WIDTH / 2;
  }
  double right = static_cast<double>(road.GetStart().x + ROAD_WIDTH / 2);
  return !(current_x < left || current_x > right);
}

bool RoadMovement::IsWithinYVerticaBounds(double current_y, const Road& road) {
  double begin_y = static_cast<double>((road.GetStart().y < road.GetEnd().y ?
    road.GetStart().y : road.GetEnd().y) - ROAD_WIDTH / 2);
  double end_y = static_cast<double>((road.GetEnd().y > road.GetStart().y ?
    road.GetEnd().y : road.GetStart().y) + ROAD_WIDTH / 2);
  return !(current_y < begin_y || current_y > end_y);
}

double RoadMovement::MoveNorthOnVerticalRoad(double current_y, const Road& road,
  double speed_y, double time, bool& is_stop_required) {
  double begin_y = static_cast<double>((road.GetStart().y < road.GetEnd().y ?
    road.GetStart().y : road.GetEnd().y) - ROAD_WIDTH / 2);
  double y = current_y + speed_y * time;
  if (y <= begin_y) {
    y = begin_y;
    is_stop_required = true;
  }
  return std::abs(current_y - y);
}

double RoadMovement::MoveSouthOnVerticalRoad(double current_y, const Road& road,
  double speed_y, double time, bool& is_stop_required) {
  double end_y = static_cast<double>((road.GetEnd().y > road.GetStart().y ?
    road.GetEnd().y : road.GetStart().y) + ROAD_WIDTH / 2);
  double y = current_y + speed_y * time;
  if (y >= end_y) {
    y = end_y;
    is_stop_required = true;
  }
  return std::abs(y - current_y);
}

double RoadMovement::MoveWestOnVerticalRoad(double current_x, const Road& road,
  double speed_x, double time, bool& is_stop_required) {
  double left = static_cast<double>(road.GetStart().x - ROAD_WIDTH / 2);
  double x = current_x + speed_x * time;
  if (x <= left) {
    x = left;
    is_stop_required = true;
  }
  if (x < -ROAD_WIDTH / 2) {
    x = -ROAD_WIDTH / 2;
  }
  return std::abs(current_x - x);
}

double RoadMovement::MoveEastOnVerticalRoad(double current_x, const Road& road,
  double speed_x, double time, bool& is_stop_required) {
  double right = static_cast<double>(road.GetStart().x + ROAD_WIDTH / 2);
  double x = current_x + speed_x * time;
  if (x >= right) {
    x = right;
    is_stop_required = true;
  }
  return std::abs(x - current_x);
}

InfoMovement RoadMovement::MoveOnHorizontalRoad(double time, const Road& road,
  Direction& direction, Speed& speed, Position& position) {
  bool is_stop_required = false;

  if (!IsWithinYHorizontalBounds(position.y, road)) {
    return { 0.0, true };
  }


  if (!IsWithinXHorizontalBounds(position.x, road)) {
    return { 0.0, true };
  }


  switch (direction) {
  case NORTH:
    return { MoveNorthOnHorizontalRoad(position.y, road, speed.y,
      time, is_stop_required), is_stop_required };
  case SOUTH:
    return { MoveSouthOnHorizontalRoad(position.y, road, speed.y,
      time, is_stop_required), is_stop_required };
  case WEST:
    return { MoveWestOnHorizontalRoad(position.x, road, speed.x,
      time, is_stop_required), is_stop_required };
  case EAST:
    return { MoveEastOnHorizontalRoad(position.x, road, speed.x,
      time, is_stop_required), is_stop_required };
  }

  return { 0.0, true };
}

bool RoadMovement::IsWithinYHorizontalBounds(double current_y, const Road& road) {
  double down = static_cast<double>(road.GetStart().y - ROAD_WIDTH / 2);
  double up = static_cast<double>(road.GetStart().y + ROAD_WIDTH / 2);
  if (down < -0.4) {
    down = -0.4;
  }
  return !(current_y < down || current_y > up);
}

bool RoadMovement::IsWithinXHorizontalBounds(double current_x, const Road& road) {
  double begin_x = static_cast<double>((road.GetStart().x < road.GetEnd().x ?
    road.GetStart().x : road.GetEnd().x) - ROAD_WIDTH / 2);
  double end_x = static_cast<double>((road.GetEnd().x > road.GetStart().x ?
    road.GetEnd().x : road.GetStart().x) + ROAD_WIDTH / 2);
  return !(current_x < begin_x || current_x > end_x);
}

double RoadMovement::MoveNorthOnHorizontalRoad(double current_y, const Road& road,
  double speed_y, double time, bool& is_stop_required) {
  double down = static_cast<double>(road.GetStart().y - ROAD_WIDTH / 2);
  double y = current_y + speed_y * time;

  if (y <= down) {
    y = down;
    is_stop_required = true;
  }

  return std::abs(current_y - y);
}

double RoadMovement::MoveSouthOnHorizontalRoad(double current_y, const Road& road,
  double speed_y, double time, bool& is_stop_required) {
  double up = static_cast<double>(road.GetStart().y + ROAD_WIDTH / 2);
  double y = current_y + speed_y * time;

  if (y >= up) {
    y = up;
    is_stop_required = true;
  }

  return std::abs(y - current_y);
}

double RoadMovement::MoveWestOnHorizontalRoad(double current_x, const Road& road,
  double speed_x, double time, bool& is_stop_required) {
  double begin_x = static_cast<double>((road.GetStart().x < road.GetEnd().x ?
    road.GetStart().x : road.GetEnd().x) - ROAD_WIDTH / 2);
  double x = current_x + speed_x * time;

  if (x <= begin_x) {
    x = begin_x;
    is_stop_required = true;
  }

  return std::abs(current_x - x);
}

double RoadMovement::MoveEastOnHorizontalRoad(double current_x, const Road& road,
  double speed_x, double time, bool& is_stop_required) {
  double end_x = static_cast<double>((road.GetEnd().x > road.GetStart().x ?
    road.GetEnd().x : road.GetStart().x) + ROAD_WIDTH / 2);
  double x = current_x + speed_x * time;

  if (x >= end_x) {
    x = end_x;
    is_stop_required = true;
  }

  if (x < -ROAD_WIDTH / 2) {
    x = -ROAD_WIDTH / 2;
  }

  return std::abs(x - current_x);
}


}