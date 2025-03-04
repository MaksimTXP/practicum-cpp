#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <memory>

namespace data_transport {

	class TransportRouter {
	private:

		static constexpr double SpeedToTimeConversion = 100.0 / 6.0;
	
	public:

		TransportRouter(const RouterSettings& settings, const TransportCatalogue& catalogue):
			settings_(settings), catalogue_(catalogue) {
			BuildGraph();
		}

		const std::optional<TransportRouteInfo> GetRouter(const std::string_view stop_from,
			const std::string_view stop_to) const;


	private:


		void BuildGraph();
		void AddVertices();
		void AddEdges();

		RouterSettings settings_;
		const TransportCatalogue& catalogue_;
		Graph graph_;
		VertexIds stop_ids_;
		RouterPtr router_;
	};

}