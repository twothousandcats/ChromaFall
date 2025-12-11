#pragma once
#include <vector>
#include <memory>

#include "components/Upgrades.hpp"
#include "data/PowerUpType.hpp"

class Entity;

class PowerUpSystem {
public:
    std::vector<PowerUpType> generateOptions(
        int count,
        WeaponType currentWeapon
    );

    void apply(Entity *player, PowerUpType type);
};
