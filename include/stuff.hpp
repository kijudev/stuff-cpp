#pragma once

#include <array>
#include <cassert>
#include <memory>
#include <utility>
#include <vector>

#include "part.hpp"
#include "slot_map.hpp"
#include "sparse_set.hpp"
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
    void attach_part(Thing thing, const T& part) {
        emplace_part<T, const T&>(thing, part);
    }

    template <typename T>
    void attach_part(Thing thing, T&& part) {
        emplace_part<T, T&&>(thing, std::move(part));
    }

    template <typename T, typename... Args>
    void emplace_part(Thing thing, Args&&... args) {
        assert(check_thing(thing));
        if (!check_thing(thing)) return;

        U32 part_idx = TypeIdx::get<T>();
        m_signatures[thing.idx].set(part_idx);

        if (!m_parts[part_idx]) {
            m_parts[part_idx] = std::make_unique<impl::SparseSet<T>>();
        }

        impl::SparseSet<T>* sparse_set =
            static_cast<impl::SparseSet<T>*>(m_parts[part_idx].get());

        sparse_set->emplace(thing, std::forward<Args>(args)...);
    };

    template <typename T>
    void remove_part(Thing thing) {
        assert(check_thing(thing));
        if (!check_thing(thing)) return;

        U32 part_idx = TypeIdx::get<T>();
        m_signatures[thing.idx].reset(part_idx);

        if (!m_parts[part_idx]) return;
        m_parts[part_idx]->remove(thing);
    }

    template <typename T>
    bool check_part(Thing thing) {
        assert(check_thing(thing));
        if (!check_thing(thing)) return false;

        U32 part_idx = TypeIdx::get<T>();
        return m_signatures[thing.idx].test(part_idx);
    }

    template <typename T>
    T& get_part(Thing thing) {
        assert(check_thing(thing));
        assert(check_part<T>(thing));

        U32 part_idx = TypeIdx::get<T>();

        impl::SparseSet<T>* sparse_set =
            static_cast<impl::SparseSet<T>*>(m_parts[part_idx].get());

        return sparse_set->get(thing);
    }

   private:
    impl::SlotMap m_things;
    std::vector<Signature> m_signatures {};
    std::array<std::unique_ptr<impl::SparseSetI>, MAX_PARTS> m_parts {};
};
}  // namespace ecs
