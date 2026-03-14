#pragma once

#include <cassert>
#include <utility>
#include <vector>

#include "thing.hpp"
#include "typedefs.hpp"

namespace ecs::impl {
class SparseSetI {
   public:
    virtual ~SparseSetI() = default;

    virtual void remove(Thing thing) = 0;
    virtual void clear()             = 0;
};

template <typename T>
class SparseSet : public SparseSetI {
   public:
    T& get(Thing thing) {
        assert(contains(thing));

        U32 dense_idx = m_sparse[thing.idx];
        return m_dense_data[dense_idx];
    }

    bool contains(Thing thing) const {
        return thing.idx < m_sparse.size() && m_sparse[thing.idx] != 0;
    }

    void insert(Thing thing, const T& part) { emplace(thing, part); }
    void insert(Thing thing, T&& part) { emplace(thing, std::move(part)); }

    template <typename... Args>
    void emplace(Thing thing, Args&&... args) {
        if (contains(thing)) {
            U32 existing_idx           = m_sparse[thing.idx];
            m_dense_data[existing_idx] = T(std::forward<Args>(args)...);
            return;
        }
        if (thing.idx >= m_sparse.size()) {
            m_sparse.resize(thing.idx + 1);
        }

        U32 dense_idx       = m_dense_data.size();
        m_sparse[thing.idx] = dense_idx;
        m_dense_idxs.push_back(thing.idx);
        m_dense_data.emplace_back(std::forward<Args>(args)...);
    }

    void remove(Thing thing) override {
        if (!contains(thing)) return;

        U32 curr_dense_idx = m_sparse[thing.idx];
        U32 last_dense_idx = m_dense_idxs.size() - 1;

        if (curr_dense_idx != last_dense_idx) {
            m_dense_data[curr_dense_idx] = std::move(m_dense_data[last_dense_idx]);
            m_dense_idxs[curr_dense_idx] = m_dense_idxs[last_dense_idx];

            U32 moved_thing_idx       = m_dense_idxs[curr_dense_idx];
            m_sparse[moved_thing_idx] = curr_dense_idx;
        }

        m_sparse[thing.idx] = 0;
        m_dense_idxs.pop_back();
        m_dense_data.pop_back();
    }

    void clear() override {
        m_sparse.clear();
        m_dense_idxs.clear();
        m_dense_data.clear();

        m_sparse.push_back(0);
        m_dense_idxs.push_back(0);
        m_dense_data.push_back(T());
    }

   private:
    std::vector<U32> m_sparse { 0 };
    std::vector<U32> m_dense_idxs { 0 };
    std::vector<T> m_dense_data { T() };
};
}  // namespace ecs::impl
