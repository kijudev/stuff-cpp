#include <print>

#include "../../include/slot_map.hpp"

int main() {
    ecs::impl::SlotMap slot_map;

    ecs::impl::Handle a = slot_map.handout();
    ecs::impl::Handle b = slot_map.handout();
    ecs::impl::Handle c = slot_map.handout();

    slot_map.release(b);
    slot_map.release(a);

    ecs::impl::Handle d = slot_map.handout();

    std::println("c -> {}", c.format());
    std::println("d -> {}", d.format());
}
