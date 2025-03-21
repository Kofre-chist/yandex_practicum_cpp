#pragma once

#include "geo.h"

#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <utility>
#include <functional>

namespace transport_catalogue {

  struct Stop {
    std::string name_stop;
    transport_catalogue::geo::Coordinates coordinates;
  };

  struct Bus {
    std::string name_bus;
    std::vector<Stop*> bus_road;
  };

  struct BusInfo {
    std::size_t stops_count;
    std::size_t unique_stop_count;
    int length;
    double curvature;
  };

  struct StopPairHasher {
    size_t operator()(const std::pair<const Stop*, const Stop*>& stop_pair) const {
      size_t hash1 = std::hash<const void*>{}(stop_pair.first);
      size_t hash2 = std::hash<const void*>{}(stop_pair.second);
      return hash1 ^ (hash2 << 1);
    }
  };

  class TransportCatalogue {
  public:
    // добавление остановки в базу
    void AddStop(const Stop& stop);

    // поиск остановки по имени
    Stop* FindStop(std::string_view name_stop) const;

    // добавление расстояния между остановками
    void SetDistance(const Stop* from, const Stop* to, int distance);

    // добавление маршрута в базу
    void AddBus(const Bus& bus);

    // получение информации о маршруте
    BusInfo GetBusInfo(std::string_view bus_name) const;

    // получение информации об останоновке
    std::string GetStopInfo(std::string_view bus_name) const;

  private:
    // получение расстояния между остановками
    int GetDistance(const Stop* from, const Stop* to) const;
    // поиск маршрута по имени
    Bus* FindBus(std::string_view bus_name) const;

    // остановки
    std::deque<Stop*> stops_;
    // индекс остановок
    std::unordered_map<std::string_view, Stop*> stopname_to_stop;
    // маршруты
    std::deque<Bus*> buses_;
    // индекс маршрутов
    std::unordered_map<std::string_view, Bus*> busname_to_bus;
    // индекс автобусов проходящих через определенную остановку
    std::unordered_map<std::string_view, std::vector<Bus*>> stopname_to_bus;
    // индекс для хранения храненения расстояния между остановками
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopPairHasher> distances;
  };
}