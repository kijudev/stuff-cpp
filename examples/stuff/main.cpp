#include <format>
#include <print>
#include <string>

#include "../../include/stuff.hpp"

struct Pos {
    F32 x {};
    F32 y {};
    F32 z {};

    std::string format() const {
        return std::format("Pos(x: {}, y: {}, z: {})", x, y, z);
    }
};

struct Health {
    F32 value {};

    std::string format() const { return std::format("Health(value: {})", value); }
};

int main() {
    ecs::Stuff stuff;

    ecs::Thing a = stuff.create_thing();
    ecs::Thing b = stuff.create_thing();

    stuff.attach_part<Pos>(a, { .x = 4.0f, .y = 2.0f, .z = 0.0f });
    stuff.attach_part<Health>(a, { .value = 21.37f });

    stuff.attach_part<Pos>(b, { .x = 6.9f, .y = -6.9f, .z = 4.2f });

    std::println("a -> (Pos) {}", stuff.check_part<Pos>(a));
    std::println("a -> (Health) {}", stuff.check_part<Health>(a));

    if (stuff.check_part<Pos>(a)) {
        Pos& pos = stuff.get_part<Pos>(a);
        std::println("a.pos = {}", pos.format());
    }

    if (stuff.check_part<Health>(a)) {
        Health& health = stuff.get_part<Health>(a);
        std::println("a.health = {}", health.format());
    }

    std::println("b -> (Pos) {}", stuff.check_part<Pos>(b));
    std::println("b -> (Health) {}", stuff.check_part<Health>(b));

    if (stuff.check_part<Pos>(b)) {
        Pos& pos = stuff.get_part<Pos>(b);
        std::println("b.pos = {}", pos.format());
    }

    if (stuff.check_part<Health>(b)) {
        Health& health = stuff.get_part<Health>(b);
        std::println("b.health = {}", health.format());
    }
}
