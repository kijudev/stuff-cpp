#include <print>

#include "../../include/slot_map.hpp"

int main() {
    ecs::SlotMap slot_map;

    ecs::Handle a = slot_map.handout();
    ecs::Handle b = slot_map.handout();
    ecs::Handle c = slot_map.handout();

    slot_map.release(b);
    slot_map.release(a);

    ecs::Handle d = slot_map.handout();

    std::println("c -> {}", c.format());
    std::println("d -> {}", d.format());
}
