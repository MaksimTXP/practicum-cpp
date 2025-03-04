#include "collision_detector.h"
#include <cassert>

namespace collision_detector {

  CollectionResult TryCollectPoint(model::Position a, model::Position b, model::Position c) {

    assert(b.x != a.x || b.y != a.y);
    const double u_x = c.x - a.x;
    const double u_y = c.y - a.y;
    const double v_x = b.x - a.x;
    const double v_y = b.y - a.y;
    const double u_dot_v = u_x * v_x + u_y * v_y;
    const double u_len2 = u_x * u_x + u_y * u_y;
    const double v_len2 = v_x * v_x + v_y * v_y;
    const double proj_ratio = u_dot_v / v_len2;
    const double sq_distance = u_len2 - (u_dot_v * u_dot_v) / v_len2;

    return CollectionResult(sq_distance, proj_ratio);
  }


  std::vector<GatheringEvent> FindGatherEvents(const ItemGathererProvider& provider) {
    const auto is_point_equal = [](model::Position point_1, model::Position point_2) {
      return point_1.x == point_2.x && point_1.y == point_2.y;
      };

    std::vector<GatheringEvent> result;

    for (size_t gatherer_id = 0; gatherer_id < provider.GatherersCount(); ++gatherer_id) {
      const Gatherer gatherer = provider.GetGatherer(gatherer_id);
      if (is_point_equal(gatherer.start_pos, gatherer.end_pos)) {
        continue;
      }
      for (size_t item_id = 0; item_id < provider.ItemsCount(); ++item_id) {
        const Item item = provider.GetItem(item_id);

        const CollectionResult collection_result = TryCollectPoint(gatherer.start_pos, gatherer.end_pos, item.position);

        if (collection_result.IsCollected(item.width + gatherer.width)) {
          result.emplace_back(GatheringEvent{
             .item_id = item_id,
             .gatherer_id = gatherer_id,
             .sq_distance = collection_result.sq_distance,
             .time = collection_result.proj_ratio,
          });
        }
      }
    }
    std::sort(result.begin(), result.end(), [](const auto& lhs, const auto& rhs) { return lhs.time < rhs.time; });
    return result;

  }
  

}  