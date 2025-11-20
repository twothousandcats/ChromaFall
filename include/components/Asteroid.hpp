#pragma once
#include "core/Component.hpp"

enum class AsteroidSize {
    LARGE,
    MEDIUM,
    SMALL
};

struct Asteroid final : Component {
    AsteroidSize size;

    explicit Asteroid(AsteroidSize s = AsteroidSize::LARGE) : size(s) {
    }
};
