#include "transport_router.h"

namespace data_transport {

	void TransportRouter::BuildGraph() {

		AddVertices();

		AddEdges();

		router_ = std::make_unique<graph::Router<double>>(graph_);

	}

	void TransportRouter::AddVertices() {

		const auto& stops = catalogue_.GetStops();

		graph_ = Graph(stops.size() * 2);

		VertexIds stop_ids;
		graph::VertexId vertex_id = 0;

		for (const auto& [stop_name, stop_info] : stops) {
			stop_ids[stop_info->name_stop] = vertex_id;
			graph_.AddEdge({
				stop_info->name_stop,
				0,
				vertex_id,
				vertex_id + 1,
				static_cast<double>(settings_.bus_wait_time)
				});
			vertex_id += 2;
		}
		stop_ids_ = std::move(stop_ids);
	}

	void TransportRouter::AddEdges() {

		const auto& buses = catalogue_.GetBuses();

		for (const auto& [bus_name, bus_info] : buses) {
			const auto& stops = bus_info->stops;
			size_t stops_count = stops.size();
			for (size_t i = 0; i < stops_count; ++i) {
				for (size_t j = i + 1; j < stops_count; ++j) {
					const Stop* stop_from = stops[i];
					const Stop* stop_to = stops[j];
					int dist_sum = 0;
					int dist_sum_inverse = 0;
					for (size_t k = i + 1; k <= j; ++k) {
						dist_sum += catalogue_.GetDistanse(stops[k - 1], stops[k]);
						dist_sum_inverse += catalogue_.GetDistanse(stops[k], stops[k - 1]);
					}
					graph_.AddEdge({
						bus_info->name_bus,
						j - i,
						stop_ids_.at(stop_from->name_stop) + 1,
						stop_ids_.at(stop_to->name_stop),
						static_cast<double>(dist_sum) / (settings_.bus_velocity * SpeedToTimeConversion)
						});

					if (!bus_info->circular_route) {
						graph_.AddEdge({
							bus_info->name_bus,
							j - i,
							stop_ids_.at(stop_to->name_stop) + 1,
							stop_ids_.at(stop_from->name_stop),
							static_cast<double>(dist_sum_inverse) / (settings_.bus_velocity * SpeedToTimeConversion)
							});
					}
				}
			}
		}
	}


	const std::optional<TransportRouteInfo> TransportRouter::GetRouter(const std::string_view stop_from, const std::string_view stop_to) const {
		TransportRouteInfo route_info;

		auto from_id_it = stop_ids_.find(std::string(stop_from));
		auto to_id_it = stop_ids_.find(std::string(stop_to));

		if (from_id_it == stop_ids_.end() || to_id_it == stop_ids_.end()) {
			return std::nullopt;
		}

		route_info.router_info = router_->BuildRoute(from_id_it->second, to_id_it->second);

		if (!route_info.router_info) {
			return std::nullopt;
		}

		for (auto&& edge_id : route_info.router_info->edges) {

				route_info.edge_router_info.emplace_back(graph_.GetEdge(edge_id));
		}

		return route_info;
	}
	

} // namespace data_transport
