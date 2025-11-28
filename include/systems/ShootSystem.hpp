#pragma once
#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>

#include "config/BulletConfig.hpp"

class Entity;

class ShootSystem {
public:
    void update(
        std::vector<std::unique_ptr<Entity> > &bullets,
        const sf::RenderWindow &window,
        const sf::Vector2f &playerPosition,
        int bulletsCount = DEFAULT_BULLETS_COUNT,
        float spreadAngle = DEFAULT_BULLETS_SPREAD_FACTOR // shotgun
    );

private:
    sf::Clock shootClock;
    constexpr static float SHOOTING_COOLDOWN = 0.5f;
};
