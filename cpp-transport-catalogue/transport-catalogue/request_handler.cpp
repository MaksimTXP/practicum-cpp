#include "request_handler.h"


using namespace std::literals::string_literals;

//Получение инормации о маршруте 
const std::optional<data_transport::Route>
RequestHandler::GetRoute(const std::string& name_bus) const {

	const data_transport::Bus* bus = catalogue_.GetBus(name_bus);

	size_t number_stops = bus->circular_route ? bus->stops.size() :
		bus->stops.size() * 2 - 1;

	int route_length = 0;
	double geographic_length = 0.0;

	for (size_t i = 0; i < bus->stops.size() - 1; ++i) {

		auto from = bus->stops[i];
		auto to = bus->stops[i + 1];

		if (bus->circular_route) {
			route_length += catalogue_.GetDistanse(from, to);
			geographic_length += geo::ComputeDistance(from->coordinat, to->coordinat);
		}
		else {
			route_length += catalogue_.GetDistanse(from, to) + catalogue_.GetDistanse(to, from);
			geographic_length += geo::ComputeDistance(from->coordinat, to->coordinat) * 2;
		}
	}

	return data_transport::Route{ number_stops ,catalogue_.ComputeNumberStopsUnique(name_bus),
		static_cast<double>(route_length),
		route_length / geographic_length };
}

//Получение карты маршрутов
svg::Document RequestHandler::RenderMap() const {
	return renderer_.GetMapTransportCatalogue(catalogue_.GetBuses());
}

//Получение имя автобуса
const std::string RequestHandler::GetNameBus(const std::string& name_bus) const {
	if (catalogue_.GetBus(name_bus)) {
		return catalogue_.GetBus(name_bus)->name_bus;
	}
	return {};
}

//Получение имя остановки 
const std::string RequestHandler::GetNameStop(const std::string& name_stop) const {
	if (catalogue_.GetStop(name_stop)) {
		return catalogue_.GetStop(name_stop)->name_stop;
	}
	return {};
}

//Получение списка автобусов проходящих через остановку 
const std::set<std::string> RequestHandler::GetBusesToStopHandler(const std::string& name_stop) const {

	return catalogue_.GetBusesToStop(name_stop);
}

//Получение информации о маршруте
const std::optional<data_transport::TransportRouteInfo> RequestHandler::GetRouterInfo(const std::string_view stop_from,
	const std::string_view stop_to) const {

	return router_.GetRouter(stop_from, stop_to);
}
