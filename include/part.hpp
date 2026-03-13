#pragma once

#include "slot_map.hpp"
#include "typedefs.hpp"

namespace ecs {
constexpr USize max_parts = 64;
using Signature           = std::bitset<max_parts>;
}  // namespace ecs
