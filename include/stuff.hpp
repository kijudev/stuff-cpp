#pragma once

#include <array>
#include <cassert>
#include <iterator>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "part.hpp"
#include "slot_map.hpp"
#include "sparse_set.hpp"
#include "thing.hpp"
#include "typeidx.hpp"

namespace ecs {
class Stuff;

template <typename T>
using PartNoRef = std::remove_reference_t<T>;

template <typename T>
using PartBase = std::remove_cv_t<PartNoRef<T>>;

template <typename T>
using PartRef =
    std::conditional_t<std::is_const_v<PartNoRef<T>>, const PartBase<T>&, PartBase<T>&>;

template <typename... Parts>
class Query {
   public:
    class Iterator {
       public:
        using value_type        = std::tuple<Thing, PartRef<Parts>...>;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;
        using reference         = value_type;

        Iterator(Stuff* stuff, Signature with, Signature without, U32 idx)
            : m_stuff(stuff), m_with(with), m_without(without), m_idx(idx) {
            impl_advance();
        }

        reference operator*() const {
            Thing thing = m_stuff->m_things.handle(m_idx);
            return { thing, impl_get_part_ref<Parts>(thing)... };
        }

        Iterator& operator++() {
            ++m_idx;
            impl_advance();
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const {
            return m_idx == other.m_idx && m_stuff == other.m_stuff;
        }

        bool operator!=(const Iterator& other) const { return !(*this == other); }

       private:
        template <typename P>
        PartRef<P> impl_get_part_ref(Thing thing) const {
            using Base = PartBase<P>;

            if constexpr (std::is_const_v<PartNoRef<P>>) {
                return static_cast<const Base&>(m_stuff->template get_part<Base>(thing));
            } else {
                return m_stuff->template get_part<Base>(thing);
            }
        }

        void impl_advance() {
            while (m_idx < m_stuff->m_signatures.size()) {
                if (impl_matches(m_idx)) return;
                ++m_idx;
            }
        }

        bool impl_matches(U32 idx) const {
            if (!m_stuff->m_things.alive(idx)) return false;

            const Signature& sig = m_stuff->m_signatures[idx];

            if ((sig & m_with) != m_with) return false;
            if ((sig & m_without).any()) return false;
            return true;
        }

        Stuff* m_stuff {};
        Signature m_with {};
        Signature m_without {};
        U32 m_idx {};
    };

    Query(Stuff* stuff, Signature with, Signature without)
        : m_stuff(stuff), m_with(with), m_without(without) {}

    Iterator begin() { return Iterator(m_stuff, m_with, m_without, 1); }
    Iterator end() {
        return Iterator(m_stuff, m_with, m_without,
                        static_cast<U32>(m_stuff->m_signatures.size()));
    }

   private:
    Stuff* m_stuff {};
    Signature m_with {};
    Signature m_without {};
};

template <typename... Parts>
class QueryBuilder {
   public:
    QueryBuilder(Stuff* stuff) : m_stuff(stuff) { (impl_set_with<Parts>(), ...); }

    template <typename... WithParts>
    QueryBuilder& with() {
        (impl_set_with<WithParts>(), ...);
        return *this;
    }

    template <typename... WithoutParts>
    QueryBuilder& without() {
        (impl_set_without<WithoutParts>(), ...);
        return *this;
    }

    Query<Parts...> get() { return Query<Parts...>(m_stuff, m_with, m_without); }

   private:
    template <typename T>
    void impl_set_with() {
        m_with.set(TypeIdx::get<PartBase<T>>());
    }

    template <typename T>
    void impl_set_without() {
        m_without.set(TypeIdx::get<PartBase<T>>());
    }

    Stuff* m_stuff {};
    Signature m_with {};
    Signature m_without {};
};

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
    bool check_thing(Thing thing) const { return m_things.check(thing); }

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
    bool check_part(Thing thing) const {
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

    template <typename T>
    const T& get_part(Thing thing) const {
        assert(check_thing(thing));
        assert(check_part<T>(thing));

        return const_cast<Stuff*>(this)->get_part<T>(thing);
    }

    template <typename... Parts>
    QueryBuilder<Parts...> query() {
        return QueryBuilder<Parts...>(this);
    };

   private:
    template <typename... Parts>
    friend class Query;

    template <typename... Parts>
    friend class QueryBuilder;

    impl::SlotMap m_things;
    std::vector<Signature> m_signatures {};
    std::array<std::unique_ptr<impl::SparseSetI>, MAX_PARTS> m_parts {};
};

}  // namespace ecs
