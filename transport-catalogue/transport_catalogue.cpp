#include "transport_catalogue.h"
#include "geo.h"

#include <iostream>

namespace transport_catalogue {

  // добавление остановки в базу
  void TransportCatalogue::AddStop(const Stop& stop) {
    Stop* new_stop = new Stop(stop);
    stops_.push_back(new_stop);

    Stop* added_stop = stops_.back();
    std::string_view name_view(added_stop->name_stop);
    stopname_to_stop[name_view] = added_stop;
  }

  // поиск остановки по имени
  Stop* TransportCatalogue::FindStop(std::string_view name_stop) const {
    auto it = stopname_to_stop.find(name_stop);
    if (it != stopname_to_stop.end()) {
      return it->second;
    }
    return nullptr;
  }

  void TransportCatalogue::SetDistance(const Stop* from, const Stop* to, int distance) {
    distances[{from, to}] = distance;

    if (distances.find({ to, from }) == distances.end()) {
      distances[{to, from}] = distance;
    }

  }

  int TransportCatalogue::GetDistance(const Stop* from, const Stop* to) const {
    auto it_1 = distances.find({ from, to });
    if (it_1 != distances.end()) {
      return it_1->second;
    }
    auto it_2 = distances.find({ to, from });
    if (it_2 != distances.end()) {
      return it_2->second;
    }
    return 0;
  }

  // добавление маршрута в базу
  void TransportCatalogue::AddBus(const Bus& bus) {
    Bus* new_bus = new Bus(bus);
    buses_.push_back(new_bus);
    Bus* added_bus = buses_.back();
    std::string_view bus_view(added_bus->name_bus);
    for (const auto& stop : added_bus->bus_road) {
      std::string_view stop_view(stop->name_stop);
      stopname_to_bus[stop_view].push_back(added_bus);
    }

    busname_to_bus[bus_view] = added_bus;
  }

  // поиск маршрута по имени
  Bus* TransportCatalogue::FindBus(std::string_view bus_name) const {
    auto it = busname_to_bus.find(bus_name);
    if (it != busname_to_bus.end()) {
      return it->second;
    }
    return nullptr;
  }

  // получение информации о маршруте
  BusInfo TransportCatalogue::GetBusInfo(std::string_view bus_name) const {
    Bus* bus = FindBus(bus_name);
    if (!bus) {
      return { 0, 0, 0, 0 };
    }

    size_t stops_count = bus->bus_road.size();
    std::unordered_set<std::string> unique_stop_names;
    double geo_length = 0.0;
    int length = 0;
    double tmp_latitude = 0.0;
    double tmp_longitude = 0.0;
    bool is_the_first_stop = true;
    Stop* tmp_stop = nullptr;

    for (const auto& stop : bus->bus_road) {
      if (!stop) continue;

      if (is_the_first_stop) {
        tmp_latitude = stop->coordinates.lat;
        tmp_longitude = stop->coordinates.lng;
        unique_stop_names.insert(stop->name_stop);
        tmp_stop = stop;
        is_the_first_stop = false;
      }
      else {
        unique_stop_names.insert(stop->name_stop);
        geo_length += transport_catalogue::geo::ComputeDistance({ tmp_latitude, tmp_longitude }, { stop->coordinates.lat, stop->coordinates.lng });
        length += GetDistance(tmp_stop, stop);
        tmp_latitude = stop->coordinates.lat;
        tmp_longitude = stop->coordinates.lng;
        tmp_stop = stop;
      }
    }

    size_t unique_stop_count = unique_stop_names.size();
    double curvature = length / geo_length;
    return BusInfo({ stops_count, unique_stop_count, length, curvature });
  }

  // получение информации об останоновке
  std::string TransportCatalogue::GetStopInfo(std::string_view stop_name) const {
    auto it_1 = stopname_to_stop.find(stop_name);
    auto it_2 = stopname_to_bus.find(stop_name);
    if (it_1 == stopname_to_stop.end()) {
      return "Stop " + std::string(stop_name) + ": not found";
    }
    else if (it_2 == stopname_to_bus.end()) {
      return "Stop " + std::string(stop_name) + ": no buses";
    }
    std::vector<Bus*> buses_vector = it_2->second;
    std::string result = "Stop " + std::string(stop_name) + ": buses ";
    std::set<std::string> bus_names;
    bool flag = true;

    for (const auto& bus : buses_vector) {
      bus_names.insert(bus->name_bus.substr(4));
    }

    for (const auto& bus_name : bus_names) {
      if (flag) {
        result += bus_name;
        flag = false;
      }
      else {
        result += " " + bus_name;
      }
    }
    return result;
  }

}