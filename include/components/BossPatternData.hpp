#pragma once

#include <SFML/Graphics/Rect.hpp>

#include "core/Component.hpp"

struct BossPatternData final : Component {
    float timeSinceSpawn = 0.f;
    float lastAttackTime = 0.f;
    bool hasEntered = false;
    float sineStartTime = 0.f;

    sf::FloatRect movementZone;

    sf::Vector2f targetPos;
    bool targetReached = true;

    // скорость в данный момент
    sf::Vector2f approachVelocity;
    float timeSinceLastTargetChange = 0.f;
};
