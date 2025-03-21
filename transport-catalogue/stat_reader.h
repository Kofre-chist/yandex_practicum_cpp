#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"

namespace transport_catalogue {
  namespace stat_reader {
    namespace detail {
      std::string TrimStat(std::string string);
    }
    void ParseAndPrintStatBus(const TransportCatalogue& tansport_catalogue, std::string_view request,
      std::ostream& output);
    void ParseAndPrintStatStop(const TransportCatalogue& tansport_catalogue, std::string_view request,
      std::ostream& output);
    void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
      std::ostream& output);
  }
}