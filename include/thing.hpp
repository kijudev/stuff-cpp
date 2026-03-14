// TODO:
// It would be propably better to make `Thing` not a typdef of `impl::Handle`, but a struct.
// This would provide type safety. But i am too lazy to do it. It will be fine.

#pragma once

#include "slot_map.hpp"

namespace ecs {
using ThingIdx = U32;
using ThingGen = U32;
using Thing    = impl::Handle;

constexpr ThingIdx NIL_THING_IDX = impl::NIL_HANDLE_IDX;
constexpr ThingGen NIL_THING_GEN = impl::NIL_HANDLE_GEN;
constexpr Thing NIL_THING        = impl::NIL_HANDLE;
}  // namespace ecs
