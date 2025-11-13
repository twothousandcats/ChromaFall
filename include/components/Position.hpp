#pragma once
#include <SFML/System/Vector2.hpp>

#include "../core/Component.hpp"

struct Position : public Component {
    sf::Vector2f value{0.f, 0.f};

    Position(
        float x = 0.f,
        float y = 0.f
    ) : value(x, y) {
    }
};
