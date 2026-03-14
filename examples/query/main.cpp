#include <format>
#include <print>
#include <string>

#include "../../include/stuff.hpp"

struct Pos {
    F32 x {};
    F32 y {};

    std::string format() const { return std::format("Pos(x: {}, y: {})", x, y); }
};

struct Vel {
    F32 dx {};
    F32 dy {};

    std::string format() const { return std::format("Vel(dx: {}, dy: {})", dx, dy); }
};

struct Health {
    F32 value {};

    std::string format() const { return std::format("Health(value: {})", value); }
};

struct Dead {
    std::string format() const { return std::format("Dead"); }
};

int main() {
    ecs::Stuff stuff;

    ecs::Thing a = stuff.create_thing();
    ecs::Thing b = stuff.create_thing();
    ecs::Thing c = stuff.create_thing();

    stuff.attach_part<Pos>(a, { .x = 420.0f, .y = 42.0f });
    stuff.attach_part<Vel>(a, { .dx = 2137.0f, .dy = 42.0f });
    stuff.attach_part<Health>(a, { .value = 69.0f });

    stuff.attach_part<Pos>(b, { .x = 420.0f, .y = 42.0f });
    stuff.attach_part<Health>(b, { .value = 2137.0f });

    stuff.attach_part<Pos>(c, { .x = 42.0f, .y = 69.0f });
    stuff.attach_part<Vel>(c, { .dx = 420.0f, .dy = 2137.0f });
    stuff.attach_part<Health>(c, { .value = 42.0f });
    stuff.attach_part<Dead>(c, {});

    auto q = stuff.query<Pos, const Health>().with<Vel>().without<Dead>().get();
    for (auto [thing, pos, health] : q) {
        pos.x += 10.0f;
        pos.y += 1.0f;
        std::println("thing {}.{} pos {} health {}", thing.idx, thing.gen, pos.format(),
                     health.format());
    }

    auto q2 = stuff.query<const Pos>().get();
    for (auto [thing, pos] : q2) {
        std::println("thing {}.{} pos {}", thing.idx, thing.gen, pos.format());
    }
}
