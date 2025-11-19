#pragma once
#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>

class Entity;

class ShootSystem {
public:
    void update(
        std::vector<std::unique_ptr<Entity> > &bullets,
        const sf::RenderWindow &window,
        const sf::Vector2f &playerPosition,
        int bulletsCount = 1,
        float spreadAngle = 0.f // shotgun
    );

private:
    sf::Clock shootClock;
    constexpr static float SHOOTING_COOLDOWN = 0.5f;
};
