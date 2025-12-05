#pragma once
#include <SFML/System/Vector2.hpp>

#include "config/TrapLaserConfig.hpp"
#include "core/Component.hpp"

struct TrapLaser final : Component {
    float totalLifetime = 0.f; // общее время жизни
    float activeDelay = 0.75f;  // сколько секунд ждать до активации
    float timer = 0.f; // общий таймер
    bool isActive = false; // fнносить урон?

    sf::Vector2f position;
    sf::Vector2f size = {TRAP_LASER_BASE_WIDTH, TRAP_LASER_BASE_HEIGHT};

    void update(float dt) {
        timer += dt;
        isActive = (timer >= activeDelay);
    }
};
