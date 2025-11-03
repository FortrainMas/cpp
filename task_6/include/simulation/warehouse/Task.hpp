#pragma once

#include <variant>

#include "simulation/warehouse/zone_entities/zones/ReceivingDock.hpp"

using Task = std::variant<ReceivingDockTask>;
