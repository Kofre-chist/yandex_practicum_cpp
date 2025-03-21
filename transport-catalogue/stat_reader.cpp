#include "stat_reader.h"

#include <iostream>

namespace transport_catalogue {
  namespace stat_reader {
    namespace detail {
      std::string TrimStat(std::string string) {
        const auto start = string.find_first_not_of(' ');
        if (start == string.npos) {
          return {};
        }
        return string.substr(start, string.find_last_not_of(' ') + 1 - start);
      }
    }
    void ParseAndPrintStatBus(const TransportCatalogue& tansport_catalogue, std::string_view request,
      std::ostream& output) {
      BusInfo result = tansport_catalogue.GetBusInfo(request);
      if (result.stops_count == 0) {
        output << std::string(request) + ": not found" << std::endl;
      }
      else {
        output << std::string(request) + ": " +
          std::to_string(result.stops_count) + " stops on route, " +
          std::to_string(result.unique_stop_count) + " unique stops, " +
          std::to_string(result.length) + " route length, " +
          std::to_string(result.curvature) + " curvature" << std::endl;
      }
    }

    void ParseAndPrintStatStop(const TransportCatalogue& tansport_catalogue, std::string_view request,
      std::ostream& output) {

      std::string stop_name = detail::TrimStat(std::string(request).substr(5));
      output << tansport_catalogue.GetStopInfo(stop_name) << std::endl;
    }

    void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
      std::ostream& output) {

      if (detail::TrimStat(std::string(request).substr(0, 3)) == "Bus") {
        ParseAndPrintStatBus(tansport_catalogue, request, output);
      }
      if (detail::TrimStat(std::string(request).substr(0, 4)) == "Stop") {
        ParseAndPrintStatStop(tansport_catalogue, request, output);
      }
    }

  }
}