#pragma once

#include "typedefs.hpp"

namespace ecs {
class TypeIdx {
   public:
    template <typename T>
    static U32 get() {
        static U32 idx = get_next();
        return idx;
    }

   private:
    static U32 get_next() {
        static U32 counter = 0;
        return counter++;
    }
};
}  // namespace ecs
