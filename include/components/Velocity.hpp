#pragma once
#include "../core/Component.hpp"
#include <SFML/System/Vector2.hpp>

struct  Velocity final : Component {
    sf::Vector2f value{0.f, 0.f};

    explicit Velocity(
        const float x = 0.f,
        const float y = 0.f
    ) : value(x, y) {
    }
};
