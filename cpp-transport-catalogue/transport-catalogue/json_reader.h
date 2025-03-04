#pragma once
#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_router.h"



class JsonReader {

public:

	JsonReader(std::istream& request) :
		request_(json::Load(request)) {}

	//Наполнение транспортного справочника 
	void ApplyRequestBase(data_transport::TransportCatalogue& catalogue)const;

	//Наполнение настроек визуализации карты 
	void ApplyVisualSettingsMap(renderer::MapRenderer& renderer) const;

	//Создание маршраута 
	data_transport::RouterSettings ApplyRouterSetting() const;

	//Обработка запроса 
	void ParseRequest(std::ostream& out, RequestHandler& request_handler);

private:

	//Получениее инфорамции о маршрутах 
	const json::Node PrintRoute(const json::Dict& request, RequestHandler& request_handler) const;

	//Получение информации об остановках  
	const json::Node PrintStop(const json::Dict& request, RequestHandler& request_handler) const;

	//Получение информации о картах 
	const json::Node PrintMap(const json::Dict& request, RequestHandler& request_handler) const;
	
	//Получение маршрура
	const json::Node PrintRouter(const json::Dict& request, RequestHandler request_handler) const;

	//Добавленеие остановок
	void SetStops(data_transport::TransportCatalogue& catalogue) const;

	//Добавление дитсанции 
	void SetDistances(data_transport::TransportCatalogue& catalogue) const;

	//Добавление автобусов
	void SetBuses(data_transport::TransportCatalogue& catalogue) const;

	//Запрос
	json::Document request_;
};