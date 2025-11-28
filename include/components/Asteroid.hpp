#pragma once
#include "core/Component.hpp"
#include "data/AsteroidSize.hpp"

struct Asteroid final : Component {
    AsteroidSize size;

    explicit Asteroid(AsteroidSize s = AsteroidSize::LARGE) : size(s) {
    }
};
