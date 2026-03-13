#include <print>

#include "../../include/stuff.hpp"

struct Pos {
    F32 x {};
    F32 y {};
    F32 z {};
};

struct Health {
    F32 value {};
};

int main() {
    ecs::Stuff stuff;

    ecs::Thing a = stuff.create_thing();
    ecs::Thing b = stuff.create_thing();

    stuff.attach_part<Pos>(a);
    stuff.attach_part<Health>(a);

    stuff.attach_part<Pos>(b);

    std::println("a -> (Pos) {}", stuff.check_part<Pos>(a));
    std::println("a -> (Health) {}", stuff.check_part<Health>(a));

    std::println("b -> (Pos) {}", stuff.check_part<Pos>(b));
    std::println("b -> (Health) {}", stuff.check_part<Health>(b));
}
