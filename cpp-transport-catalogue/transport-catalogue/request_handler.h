#pragma once 

#include <optional> 
#include <fstream> 

#include "transport_catalogue.h" 
#include "map_renderer.h" 
#include "transport_router.h"



class RequestHandler {

public:

	explicit RequestHandler(data_transport::TransportCatalogue& catalogue, renderer::MapRenderer& renderer, 
		const data_transport::TransportRouter& router)
		: catalogue_(catalogue), renderer_(renderer), router_(router) {
	}

	//Получение инормации о маршруте 
	const std::optional<data_transport::Route> GetRoute(const std::string& name_bus) const;
	//Получение карты маршрутов
	svg::Document RenderMap() const;
	//Получение имя автобуса 
	const std::string GetNameBus(const std::string& name_bus) const;
	//Получение имя остановки 
	const std::string GetNameStop(const std::string& name_stop) const;
	//Получение списка автобусов проходящих через остановку 
	const std::set<std::string> GetBusesToStopHandler(const std::string& name_stop) const;
	//Получение маршрута
	const std::optional<data_transport::TransportRouteInfo> GetRouterInfo (const std::string_view stop_from,
		const std::string_view stop_to) const;

private:

	const data_transport::TransportCatalogue& catalogue_;
	const renderer::MapRenderer& renderer_;
	const data_transport::TransportRouter& router_;

};