#include <sstream> 

#include "json_reader.h" 
#include "json_builder.h"

using namespace std::literals::string_literals;

//Наполнение транспортного справочника 
void JsonReader::ApplyRequestBase(data_transport::TransportCatalogue& catalogue) const {
	//Добавление остановок 
	SetStops(catalogue);
	//Добавление дистанций 
	SetDistances(catalogue);
	//Добавление автобусов 
	SetBuses(catalogue);
}

//Добавление остановок 
void JsonReader::SetStops(data_transport::TransportCatalogue& catalogue)const {
	const auto& base_requests = request_.GetRoot().AsMap().at("base_requests").AsArray();
	for (const auto& map : base_requests) {
		const auto& type = map.AsMap().at("type").AsString();
		if (type == "Stop") {
			catalogue.AddStop(map.AsMap().at("name").AsString(),
				{ map.AsMap().at("latitude").AsDouble(),
				 map.AsMap().at("longitude").AsDouble() });
		}
	}
}

//Добавление дистанций 
void JsonReader::SetDistances(data_transport::TransportCatalogue& catalogue)const {

	const auto& base_requests = request_.GetRoot().AsMap().at("base_requests").AsArray();
	for (const auto& map : base_requests) {
		const auto& type = map.AsMap().at("type").AsString();
		if (type == "Stop" && map.AsMap().count("road_distances")) {
			const auto& distances = map.AsMap().at("road_distances").AsMap();
			const auto& stop_name = map.AsMap().at("name").AsString();
			for (const auto& distance : distances) {
				catalogue.AddDistanse(catalogue.GetStop(stop_name),
					catalogue.GetStop(distance.first),
					distance.second.AsInt());
			}
		}
	}
}

//Добавление автобусов 
void JsonReader::SetBuses(data_transport::TransportCatalogue& catalogue)const {

	const auto& base_requests = request_.GetRoot().AsMap().at("base_requests").AsArray();
	for (const auto& map : base_requests) {
		const auto& type = map.AsMap().at("type").AsString();
		if (type == "Bus") {
			std::vector<const data_transport::Stop*> stops;
			for (const auto& name : map.AsMap().at("stops").AsArray()) {
				stops.push_back(catalogue.GetStop(name.AsString()));
			}
			catalogue.AddBus(map.AsMap().at("name").AsString(), stops,
				map.AsMap().at("is_roundtrip").AsBool());
		}
	}
}

// Функция для обработки underlayer_color 
svg::Color ExtractUnderlayerColor(const json::Node& underlayer_color) {
	if (underlayer_color.IsString()) {
		return underlayer_color.AsString();
	}
	else if (underlayer_color.IsArray()) {
		const auto type_color = underlayer_color.AsArray();
		if (type_color.size() == 3) {
			return svg::Rgb(type_color[0].AsInt(), type_color[1].AsInt(), type_color[2].AsInt());
		}
		else if (type_color.size() == 4) {
			return svg::Rgba(type_color[0].AsInt(), type_color[1].AsInt(),
				type_color[2].AsInt(), type_color[3].AsDouble());
		}
	}
	throw std::runtime_error("Invalid underlayer_color format in JSON.");

}

// Функция для обработки color_palette 
std::vector<svg::Color> ExtractColorPalette(const json::Array& color_palette) {

	std::vector<svg::Color> palette;
	for (auto&& color : color_palette) {
		if (color.IsString()) {
			palette.push_back(color.AsString());
		}
		else if (color.IsArray()) {
			const auto type_color_arr = color.AsArray();
			if (type_color_arr.size() == 3) {
				palette.emplace_back(svg::Rgb(type_color_arr[0].AsInt(),
					type_color_arr[1].AsInt(), type_color_arr[2].AsInt()));
			}
			else if (type_color_arr.size() == 4) {
				palette.emplace_back(svg::Rgba(type_color_arr[0].AsInt(),
					type_color_arr[1].AsInt(), type_color_arr[2].AsInt(),
					type_color_arr[3].AsDouble()));
			}
		}
	}
	return palette;

}

//Наполнение настроек визуализации карты 
void JsonReader::ApplyVisualSettingsMap(renderer::MapRenderer& renderer) const {
	const auto& request = request_.GetRoot().AsMap().at("render_settings").AsMap();
	
	renderer::VisualSettingsMap res;
	res.width = request.at("width").AsDouble();
	res.height = request.at("height").AsDouble();
	res.padding = request.at("padding").AsDouble();
	res.line_width = request.at("line_width").AsDouble();
	res.stop_radius = request.at("stop_radius").AsDouble();
	res.bus_label_font_size = request.at("bus_label_font_size").AsInt();
	res.bus_label_offset = { request.at("bus_label_offset").AsArray()[0].AsDouble(),
							 request.at("bus_label_offset").AsArray()[1].AsDouble() };
	res.stop_label_font_size = request.at("stop_label_font_size").AsInt();
	res.stop_label_offset = { request.at("stop_label_offset").AsArray()[0].AsDouble(),
							  request.at("stop_label_offset").AsArray()[1].AsDouble() };
	const json::Node& underlayer_color = request.at("underlayer_color");
	res.underlayer_color = ExtractUnderlayerColor(underlayer_color);
	res.underlayer_width = request.at("underlayer_width").AsDouble();
	const json::Array& color_palette = request.at("color_palette").AsArray();
	res.color_palette = ExtractColorPalette(color_palette);
	renderer.AddVisualSettingsMap(res);
}

//Применить настройки маршраута 
data_transport::RouterSettings JsonReader::ApplyRouterSetting() const {
	const auto& request = request_.GetRoot().AsMap().at("routing_settings").AsMap();
	return   {request.at("bus_wait_time").AsInt(), request.at("bus_velocity"s).AsDouble()} ;
}

//Обработка запроса 
void JsonReader::ParseRequest(std::ostream& out, RequestHandler& request_handler) {

	const auto& stat_requests = request_.GetRoot().AsMap().at("stat_requests").AsArray();
	json::Array res;
	res.reserve(stat_requests.size());
	for (auto&& request : stat_requests) {
		if (request.AsMap().at("type").AsString() == "Stop") {
			res.emplace_back(PrintStop(request.AsMap(), request_handler));
		}
		if (request.AsMap().at("type").AsString() == "Bus") {
			res.emplace_back(PrintRoute(request.AsMap(), request_handler));
		}
		if (request.AsMap().at("type").AsString() == "Map") {
			res.emplace_back(PrintMap(request.AsMap(), request_handler));
		}
		if (request.AsMap().at("type").AsString() == "Route") {
			res.emplace_back(PrintRouter(request.AsMap(), request_handler));
		}
	}
	json::Print(json::Document{ std::move(res) }, out);

}

//Получениее инфорамции о маршрутах 
const json::Node JsonReader::PrintRoute(const json::Dict& request, RequestHandler& request_handler) const {
	json::Builder builder;
	auto res = builder.StartDict();

	res.Key("request_id")
		.Value(request.at("id").AsInt());
	auto bus_name = request.at("name").AsString();

	if (request_handler.GetNameBus(bus_name).empty()) {
		res.Key("error_message")
			.Value("not found");
	}
	else {
		auto route = request_handler.GetRoute(bus_name);
		res.Key("curvature")
			.Value(route->curvature);
		res.Key("route_length")
			.Value(route->route_length);
		res.Key("stop_count")
			.Value(static_cast<int>(route->number_stops));
		res.Key("unique_stop_count")
			.Value(static_cast<int>(route->number_stops_unique));
	}

	return builder.EndDict()
		.Build();
}

//Получение информации об остановках  
const json::Node JsonReader::PrintStop(const json::Dict& request, RequestHandler& request_handler) const {
	const std::string& stop_name = request.at("name").AsString();
	const int request_id = request.at("id").AsInt();

	json::Builder builder;
	if (request_handler.GetNameStop(stop_name).empty()) {
		return builder
			.StartDict()
			.Key("request_id").Value(request_id)
			.Key("error_message").Value("not found")
			.EndDict()
			.Build();
	}
	else {
		json::Array buses;
		const auto& buses_to_stop = request_handler.GetBusesToStopHandler(stop_name);
		buses.reserve(buses_to_stop.size());
		for (auto& bus : buses_to_stop) {
			buses.emplace_back(bus);
		}
		return builder
			.StartDict()
			.Key("request_id").Value(request_id)
			.Key("buses").Value(buses)
			.EndDict()
			.Build();
	}
}

//Получение информации о картах 
const json::Node JsonReader::PrintMap(const json::Dict& request, RequestHandler& request_handler) const {
	json::Builder builder;

	auto res = builder.StartDict();
	res.Key("request_id")
		.Value(request.at("id").AsInt());

	std::ostringstream oss;
	svg::Document map = request_handler.RenderMap();
	map.Render(oss);
	res.Key("map")
		.Value(oss.str());

	return res.EndDict()
		.Build();
}

//Получение маршрура
const json::Node JsonReader::PrintRouter(const json::Dict& request, RequestHandler request_handler) const {
	json::Node result;
	const int id = request.at("id"s).AsInt();
	const std::string_view stop_from = request.at("from"s).AsString();
	const std::string_view stop_to = request.at("to"s).AsString();
	const auto& routing = request_handler.GetRouterInfo(stop_from, stop_to);

	if (!routing) {
		result = json::Builder{}
			.StartDict()
			.Key("request_id"s).Value(id)
			.Key("error_message"s).Value("not found"s)
			.EndDict()
			.Build();
	}
	else {
		json::Array items;
		double total_time = 0.0;
		items.reserve(routing.value().router_info->edges.size());
		for (auto& edge : routing.value().edge_router_info) {
			if (edge.weight_edge == 0) {
				items.emplace_back(json::Node(json::Builder{}
					.StartDict()
					.Key("stop_name"s).Value(edge.name)
					.Key("time"s).Value(edge.weight)
					.Key("type"s).Value("Wait"s)
					.EndDict()
					.Build()));

				total_time += edge.weight;
			}
			else {
				items.emplace_back(json::Node(json::Builder{}
					.StartDict()
					.Key("bus"s).Value(edge.name)
					.Key("span_count"s).Value(static_cast<int>(edge.weight_edge))
					.Key("time"s).Value(edge.weight)
					.Key("type"s).Value("Bus"s)
					.EndDict()
					.Build()));

				total_time += edge.weight;
			}
		}

		result = json::Builder{}
			.StartDict()
			.Key("request_id"s).Value(id)
			.Key("total_time"s).Value(total_time)
			.Key("items"s).Value(items)
			.EndDict()
			.Build();
	}

	return result;

}