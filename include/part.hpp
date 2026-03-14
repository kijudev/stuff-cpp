#pragma once

#include "typedefs.hpp"

namespace ecs {
constexpr USize MAX_PARTS = 64;

using Signature = std::bitset<MAX_PARTS>;
}  // namespace ecs
