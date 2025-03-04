
#include "transport_catalogue.h"
#include <iostream>


namespace data_transport {

	// Добавление маршрута
	void TransportCatalogue::AddBus(const std::string& name_bus,
		std::vector<const Stop*> stops, bool circular_route)
	{
		all_buses_.push_back({ name_bus, stops, circular_route });
		for (const auto& stop_name : stops) {
			buses_to_stop_.at(stop_name->name_stop).insert(name_bus);
		}
		buses_[all_buses_.back().name_bus] = &all_buses_.back();
	}

	// Добавление остановки
	void TransportCatalogue::AddStop(const std::string& name_stop,
		const geo::Coordinates& coordinates)
	{
		all_stops_.push_back({ name_stop, coordinates });
		buses_to_stop_[std::string_view(all_stops_.back().name_stop)];
		stops_[std::string_view(all_stops_.back().name_stop)] = &all_stops_.back();
	}

	//Добавление дистанции
	void TransportCatalogue::AddDistanse(const Stop* from,
		const Stop* to, const int distanse) {
		stop_distance_[{from, to}] = distanse;
	}

	// Получение информации о маршруте
	const Bus* TransportCatalogue::GetBus(const std::string& name) const
	{
		auto it = buses_.find(name);
		return it != buses_.end() ? it->second : nullptr;
	}

	// Получение информации об остановке
	const Stop* TransportCatalogue::GetStop(const std::string& name) const
	{
		auto it = stops_.find(name);
		return it != stops_.end() ? it->second : nullptr;
	}

	//Получение дистанции
	int TransportCatalogue::GetDistanse(const Stop* from, const Stop* to) const {
		auto it_f_t = stop_distance_.find({ from, to });
		if (it_f_t != stop_distance_.end()) {
			return it_f_t->second;
		}
		else {
			auto it_t_f = stop_distance_.find({ to, from });
			if (it_t_f != stop_distance_.end()) {
				return it_t_f->second;
			}
		}
		return 0;
	}

	// Вычисление количества уникальных остановок на маршруте
	size_t TransportCatalogue::ComputeNumberStopsUnique(const std::string& name_bus)const
	{
		auto it = buses_.find(name_bus);
		if (it != buses_.end()) {
			std::unordered_set<std::string_view> tmp;
			for (auto&& stop : it->second->stops) {
				tmp.insert(stop->name_stop);
			}
			return tmp.size();
		}
		else {
			return 0;
		}
	}

	//Получение списка автобусов проходящих через остановку
	const std::set<std::string>
		TransportCatalogue::GetBusesToStop(const std::string& name_stop) const
	{
		return buses_to_stop_.count(name_stop) > 0 ?
			buses_to_stop_.at(name_stop) : std::set<std::string>{};
	}

	//Получение всех автобусных маршрутов
	const std::map<std::string_view, const Bus*> TransportCatalogue::GetBuses() const {
		std::map<std::string_view, const Bus*> res;
		for (const auto& bus : buses_) {
			res.emplace(bus);
		}
		return res;
	}

	//Получение остановок
	const std::map<std::string_view, const Stop*> TransportCatalogue::GetStops() const {
		std::map<std::string_view, const Stop*> res;
		for (const auto& stop : stops_) {
			res.emplace(stop);
		}
		return res;
	}

}