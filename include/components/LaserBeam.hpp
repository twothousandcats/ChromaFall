#pragma once

#include "core/Component.hpp"
#include <SFML/System/Vector2.hpp>

struct LaserBeam final : Component {
    float damage = 0.f;
    sf::Vector2f endPoint;
};