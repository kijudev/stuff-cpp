// SlotMap is a data structure that manages handles.
// Handle is simply a struct that contains:
//      id              : index to the array
//      generation      : generation count
// The generation count is used to keep track of the number of times a specific index has
// been used. Once a handle is removed from the SlotMap, the index is added to the free
// list. If a new handle is handed out from the free list the generation count of it is
// incremented. That way we can know if any handle is valid at any time - we can check it
// agaist the current state of the SlotMap, If generation counts match, it is a valid
// handle, if not it is invalid.
// Future design:
//      - templated handle: type safety if needed

#pragma once

#include <format>
#include <string>
#include <vector>

#include "typedefs.hpp"

namespace ecs::impl {

// =========================================================================================
// Declaration
// =========================================================================================

struct Handle {
    U32 idx { 0 };
    U32 gen { 0 };

    bool operator==(Handle other) const;
    std::string format() const;
};

constexpr U32 NIL_HANDLE_IDX = 0;
constexpr U32 NIL_HANDLE_GEN = 0;
constexpr Handle NIL_HANDLE  = { .idx = NIL_HANDLE_IDX, .gen = NIL_HANDLE_GEN };

class SlotMap {
   public:
    Handle handout();
    void release(Handle handle);
    bool check(Handle handle) const;
    USize size() const;

   private:
    std::vector<U32> m_slots { NIL_HANDLE_GEN };
    std::vector<U32> m_free {};
};

// =========================================================================================
// Implementation
// =========================================================================================

inline bool Handle::operator==(Handle other) const {
    return idx == other.idx && gen == other.gen;
}

inline std::string Handle::format() const {
    return std::format("ecs::Handle (idx: {}, gen: {})", idx, gen);
}

inline Handle SlotMap::handout() {
    if (m_free.empty()) {
        Handle h {
            .idx = static_cast<U32>(m_slots.size()),
            .gen = 1,
        };

        m_slots.push_back(h.gen);
        return h;
    } else {
        U32 idx = m_free.back();
        m_free.pop_back();

        U32& slot = m_slots[idx];
        slot += 3;

        return {
            .idx = idx,
            .gen = slot,
        };
    }
}

inline void SlotMap::release(Handle handle) {
    if (!check(handle)) return;

    m_slots[handle.idx] &= 0xFFFFFFFE;
    m_free.push_back(handle.idx);
}

inline bool SlotMap::check(Handle handle) const {
    if (handle == NIL_HANDLE) return false;
    if (static_cast<USize>(handle.idx) >= m_slots.size()) return false;
    if (handle.gen != m_slots[handle.idx]) return false;
    return true;
}

inline USize SlotMap::size() const { return m_slots.size() - m_free.size() - 1; }
}  // namespace ecs::impl
