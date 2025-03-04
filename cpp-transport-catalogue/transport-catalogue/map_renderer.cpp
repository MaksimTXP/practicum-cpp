#include "map_renderer.h"

using namespace std::literals::string_literals;

bool IsZero(double value) {
	return std::abs(value) < EPSILON;
}

namespace renderer {

	//Добавление настроеек
	void MapRenderer::AddVisualSettingsMap(const VisualSettingsMap& setting) {
		setting_ = setting;
	}

	//Инициализация подсло 
	svg::Text MapRenderer::InitUnderlayer(const svg::Text& name_text) const {
		svg::Text underlayer = name_text;
		underlayer.SetFillColor(setting_.underlayer_color);
		underlayer.SetStrokeColor(setting_.underlayer_color);
		underlayer.SetStrokeWidth(setting_.underlayer_width);
		underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
		underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
		return underlayer;
	}


	//Получениие линии маршрута
	std::vector<svg::Polyline> MapRenderer::GetLinesRouteBus(const Buses& buses,
		const SphereProjector& sphere_projector) const {

		std::vector<svg::Polyline> res;
		size_t number_color = 0;

		for (const auto& [name_bus, bus] : buses) {
			if (!bus->stops.empty()) {
				svg::Polyline line;

				for (auto it = bus->stops.begin(); it != bus->stops.end(); ++it) {
					line.AddPoint(sphere_projector((*it)->coordinat));
				}

				if (!bus->circular_route) {
					for (auto it = std::next(bus->stops.rbegin()); it != bus->stops.rend(); ++it) {
						line.AddPoint(sphere_projector((*it)->coordinat));
					}
				}

				line.SetStrokeColor(setting_.color_palette[number_color]);
				line.SetFillColor("none"s);
				line.SetStrokeWidth(setting_.line_width);
				line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
				line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

				res.emplace_back(std::move(line));

				number_color = (number_color + 1) % setting_.color_palette.size();
			}
		}

		return res;
	}

	//Получение имя автобуса
	std::vector<svg::Text> MapRenderer::GetNameBus(const Buses& buses,
		const SphereProjector& sphere_projector) const {
		std::vector<svg::Text> res;
		size_t number_color = 0;

		for (const auto& [bus_number, bus] : buses) {
			if (!bus->stops.empty()) {
				svg::Text name_bus;

				const auto& first_stop = bus->stops.front();
				const auto& last_stop = bus->stops.back();

				name_bus.SetPosition(sphere_projector(first_stop->coordinat));
				name_bus.SetOffset(setting_.bus_label_offset);
				name_bus.SetFontSize(setting_.bus_label_font_size);
				name_bus.SetFontFamily("Verdana");
				name_bus.SetFontWeight("bold");
				name_bus.SetData(bus->name_bus);
				name_bus.SetFillColor(setting_.color_palette[number_color]);

				svg::Text underlayer = InitUnderlayer(name_bus);

				res.emplace_back(underlayer);
				res.emplace_back(name_bus);

				if (!bus->circular_route && first_stop != last_stop) {
					res.emplace_back(underlayer.SetPosition(sphere_projector(last_stop->coordinat)));
					res.emplace_back(name_bus.SetPosition(sphere_projector(last_stop->coordinat)));
				}

				number_color = (number_color + 1) % setting_.color_palette.size();
			}
		}

		return res;
	}

	//Получение метки останоки
	std::vector<svg::Circle> MapRenderer::GetLabelStop(const Stops& stops,
		const SphereProjector& sphere_projector) const {
		std::vector<svg::Circle> res;
		for (const auto& [stop_nmae, stop] : stops) {
			svg::Circle circle;
			circle.SetCenter(sphere_projector(stop->coordinat));
			circle.SetRadius(setting_.stop_radius);
			circle.SetFillColor("white");

			res.push_back(circle);
		}
		return res;
	}


	//Получение метки имени остановки
	std::vector<svg::Text> MapRenderer::GetNameStop(const Stops& stops,
		const SphereProjector& sphere_projector) const {
		std::vector<svg::Text> res;
		svg::Text name_stop;

		for (const auto& [stop_name, stop] : stops) {
			name_stop.SetPosition(sphere_projector(stop->coordinat));
			name_stop.SetOffset(setting_.stop_label_offset);
			name_stop.SetFontSize(setting_.stop_label_font_size);
			name_stop.SetFontFamily("Verdana");
			name_stop.SetData(stop->name_stop);
			name_stop.SetFillColor("black");

			svg::Text underlayer = InitUnderlayer(name_stop);

			res.push_back(underlayer);
			res.push_back(name_stop);
		}

		return res;
	}

	//Получение карты маршрутов
	svg::Document MapRenderer::GetMapTransportCatalogue(const Buses& buses) const {

		svg::Document res;
		std::vector<geo::Coordinates> coordinate_stops;
		Stops stop_to_bus;
		for (const auto& [name_bus, bus] : buses) {
			if (!bus->stops.empty())
				for (const auto& stop : bus->stops) {
					coordinate_stops.push_back(stop->coordinat);
					stop_to_bus[stop->name_stop] = stop;
				}
		}
		SphereProjector sphere_projector(coordinate_stops.begin(), coordinate_stops.end(),
			setting_.width, setting_.height, setting_.padding);
		for (const auto& line : GetLinesRouteBus(buses, sphere_projector)) {
			res.Add(line);
		}
		for (const auto& name_bus : GetNameBus(buses, sphere_projector)) {
			res.Add(name_bus);
		}
		for (const auto& circle : GetLabelStop(stop_to_bus, sphere_projector)) {
			res.Add(circle);
		}
		for (const auto& name_stop : GetNameStop(stop_to_bus, sphere_projector)) {
			res.Add(name_stop);
		}
		return res;
	}
}