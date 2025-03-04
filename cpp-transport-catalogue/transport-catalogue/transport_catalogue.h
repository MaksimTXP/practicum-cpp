#pragma once

#include <deque>
#include <set>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <map>

#include "geo.h"
#include "domain.h"

namespace data_transport {


	//Клас транспортный справочник
	class TransportCatalogue {

	private:

		//Хеширование дистанции
		struct HasherDistanceStop {
			std::size_t operator()(const std::pair<const Stop*, const Stop*>& p) const {
				// Хеширование пары указателей
				std::size_t hash1 = std::hash<const void*>{}(p.first);
				std::size_t hash2 = std::hash<const void*>{}(p.second);

				return hash1 * 37 + hash2;
			}
		};

	public:

		// Добавление маршрута
		void AddBus(const std::string& name_bus,
			std::vector<const Stop*> stops, bool circular_route);

		// Добавление остановки
		void AddStop(const std::string& name_stop, const geo::Coordinates& coordinat);

		//Получение информации о маршруте 
		const Bus* GetBus(const std::string& name)const;

		//Получение информации об остановках
		const Stop* GetStop(const std::string& name)const;

		//Получение списка автобусов проходящих через остановку
		const std::set<std::string>
			GetBusesToStop(const std::string& name_stop) const;

		//Добавление дистанции
		void AddDistanse(const Stop* from, const Stop* to, const int distanse);

		//Получение дистанции
		int GetDistanse(const Stop* from, const Stop* to) const;

		//Вычисление количества уникальных остановок
		size_t ComputeNumberStopsUnique(const std::string& name_bus) const;

		//Получение автобусных маршрутов
		const std::map<std::string_view, const Bus*> GetBuses() const;

		//Получение остановок
		const std::map<std::string_view, const Stop*> GetStops() const;

	private:

		//Автобусы 
		std::deque<Bus> all_buses_;
		//Остановоки
		std::deque<Stop> all_stops_;
		//Хранит указатель на автобус по имени
		std::unordered_map<std::string_view, const Bus*> buses_;
		//Хранит указатль на остановки по имени
		std::unordered_map<std::string_view, const Stop*> stops_;
		//Список автобусов проходящих через остановку
		std::unordered_map<std::string_view, std::set<std::string>> buses_to_stop_;
		//Список дистанций между остановками
		std::unordered_map<std::pair<const Stop*, const Stop*>, int, HasherDistanceStop> stop_distance_;

	};

}