#pragma once

#include <cassert>
#include <vector>

#include "part.hpp"
#include "slot_map.hpp"
#include "thing.hpp"
#include "typeidx.hpp"

namespace ecs {
class Stuff {
   public:
    Thing create_thing() {
        Thing thing = m_things.handout();

        if (thing.idx >= m_signatures.size()) {
            m_signatures.resize(thing.idx + 1);
        }

        m_signatures[thing.idx].reset();
        return thing;
    }

    void destroy_thing(Thing thing) {
        if (!check_thing(thing)) return;
        m_signatures[thing.idx].reset();
        m_things.release(thing);
    }

    bool check_thing(Thing thing) { return m_things.check(thing); }

    template <typename T>
    void attach_part(Thing thing) {
        assert(check_thing(thing));
        if (!check_thing(thing)) return;

        U32 part_idx = TypeIdx::get<T>();
        m_signatures[thing.idx].set(part_idx);
    };

    template <typename T>
    void remove_part(Thing thing) {
        assert(check_thing(thing));
        if (!check_thing(thing)) return;

        U32 part_idx = TypeIdx::get<T>();
        m_signatures[thing.idx].reset(part_idx);
    }

    template <typename T>
    bool check_part(Thing thing) {
        assert(check_thing(thing));
        if (!check_thing(thing)) return false;

        U32 part_idx = TypeIdx::get<T>();
        return m_signatures[thing.idx].test(part_idx);
    }

   private:
    SlotMap m_things;
    std::vector<Signature> m_signatures {};
};
}  // namespace ecs
