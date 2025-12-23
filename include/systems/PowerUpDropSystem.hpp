#pragma once
#include <random>

#include "components/Upgrades.hpp"
#include "entities/Entity.hpp"

class PowerUpDropSystem {
public:
    PowerUpDropSystem();

    void textureFallback(
        Entity *pickup
    );

    void tryDropPowerUp(
        std::vector<std::unique_ptr<Entity> > &pickups,
        const sf::Vector2f &spawnPos,
        WeaponType currentWeapon,
        std::mt19937 &rng
    );

private:
    sf::Texture powerUpTexture;
};
