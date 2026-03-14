#pragma once

#include <cassert>
#include <utility>
#include <vector>

#include "slot_map.hpp"
#include "thing.hpp"
#include "typedefs.hpp"

namespace ecs {
namespace impl {
class SparseSetI {
   public:
    virtual ~SparseSetI() = default;

    virtual void remove(Handle thing) = 0;
    virtual void clear()              = 0;
};
}  // namespace impl

template <typename T>
class SparseSet : public impl::SparseSetI {
   public:
    T& get(Thing thing) {
        assert(thing.idx < m_sparse.size());

        U32 dense_idx = m_sparse[thing.idx];
        return m_dense_data[dense_idx];
    }

    void insert(Thing thing, const T& part) { emplace(thing, part); }
    void insert(Thing thing, T&& part) { emplace(thing, std::move(part)); }

    template <typename... Args>
    void emplace(Thing thing, Args&&... args) {
        U32 dense_idx = m_dense_data.size();

        if (m_sparse.size() < thing.idx) {
            m_sparse.resize(thing.idx + 1);
        }

        m_sparse[thing.idx]     = dense_idx;
        m_dense_idxs[dense_idx] = thing.idx;

        m_dense_data.emplace_back(std::forward(args)...);
    }

    void remove(Thing thing) override {
        U32 curr_dense_idx = m_sparse[thing.idx];
        U32 last_dense_idx = m_dense_idxs.size() - 1;

        if (curr_dense_idx != last_dense_idx) {
            m_dense_data[curr_dense_idx] = std::move(m_dense_data[last_dense_idx]);
            m_dense_idxs[curr_dense_idx] = m_dense_idxs[last_dense_idx];
        }

        m_sparse[thing.idx] = 0;
        m_dense_idxs.pop_back();
        m_dense_data.pop_back();
    }

    void clear() override {}

   private:
    std::vector<U32> m_sparse { 0 };
    std::vector<U32> m_dense_idxs { 0 };

    // 0 -> Stub
    std::vector<T> m_dense_data { T() };
};
}  // namespace ecs
