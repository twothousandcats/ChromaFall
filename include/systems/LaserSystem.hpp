#pragma once
#include <memory>
#include <SFML/Graphics/RenderWindow.hpp>

#include "config/LaserBeamConfig.hpp"

class Entity;

class LaserSystem {
public:
    void update(
        std::unique_ptr<Entity>& laserEntity,
        const sf::RenderWindow& window,
        const sf::Vector2f& playerMuzzlePosition,
        bool isShooting,
        bool isLaserWeapon,
        float damage,
        float length,
        float width = LASER_WIDTH
    );
};
