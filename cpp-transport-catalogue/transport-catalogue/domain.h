#pragma once

#include<map>

#include "geo.h"
#include "svg.h"
#include "router.h"

namespace data_transport {

	//Остановки
	struct Stop {

		//Имя остановки
		std::string name_stop;
		//Коодинаты
		geo::Coordinates coordinat;

	};

	//Информация о маршруте
	struct Bus {

		//Имя маршрута
		std::string name_bus;
		//Указатели на остановки
		std::vector<const Stop*> stops;
		//Флаг для кольцевого маршрута 
		bool circular_route;

	};


	//Параметры маршрута для ответа на запрос
	struct Route {

		//Количество остановок в маршруте
		size_t number_stops = 0;
		//Количество уникальх остановок в маршруте
		size_t number_stops_unique = 0;
		//Дистанция
		double route_length = 0;
		//Кривизна 
		double curvature = 0;

	};

	struct RouterSettings {

		int bus_wait_time = 0;
		double bus_velocity = 0.0;
	};


	using Graph = graph::DirectedWeightedGraph<double>;
	using VertexIds = std::unordered_map<std::string, graph::VertexId>;
	using RouterPtr = std::unique_ptr<graph::Router<double>>;
	using Router = graph::Router<double>::RouteInfo;
	using EdgeRouter = graph::Edge<double>;

	struct TransportRouteInfo {

		std::optional<Router> router_info;
		std::vector<EdgeRouter> edge_router_info;

	};


} //namespace data_transport 

namespace renderer {

	//Параметры настройки карты
	struct VisualSettingsMap {

		//Ширина 
		double width = 0.0;
		//Высота
		double height = 0.0;
		//Отступ краёв карты
		double padding = 0.0;
		//Толщина линий, которыми рисуются автобусные маршруты
		double line_width = 0.0;
		//Радиус окружностей, которыми обозначаются остановки
		double stop_radius = 0.0;
		//Размер текста, которым написаны названия автобусных маршрутов
		int bus_label_font_size = 0;
		//Смещение надписи с названием маршрута относительно координат конечной остановки на карте
		svg::Point bus_label_offset{ 0.0, 0.0 };
		//Размер текста, которым отображаются названия остановок
		int stop_label_font_size = 0;
		//Смещение названия остановки относительно её координат на карте
		svg::Point stop_label_offset{ 0.0, 0.0 };
		//Цвет подложки под названиями остановок и маршрутов
		svg::Color underlayer_color{ svg::NoneColor };
		//Толщина подложки под названиями остановок и маршрутов
		double underlayer_width = 0.0;
		//Цветовая палитра
		std::vector<svg::Color> color_palette{};

	};
}//namespace rederer
