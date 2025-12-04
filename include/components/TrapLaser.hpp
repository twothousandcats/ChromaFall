#pragma once
#include <SFML/System/Vector2.hpp>

#include "core/Component.hpp"

struct TrapLaser final : Component {
    float lifetime = 0.0f; // время жизни
    float activeDuration = 0.0f; // время активности
    bool isActive = true; // флаг активности

    sf::Vector2f position;
    sf::Vector2f size = {10.0f, 800.0f};
};
