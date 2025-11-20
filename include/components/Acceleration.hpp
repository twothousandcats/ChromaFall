#pragma once
#include <SFML/System/Vector2.hpp>

#include "core/Component.hpp"


struct Acceleration final : Component {
    sf::Vector2f value{0.f, 0.f};

    explicit Acceleration(
        float x = 0.f,
        float y = 0.f
    ) : value(x, y) {
    };
};
