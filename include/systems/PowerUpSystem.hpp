#pragma once
#include <vector>
#include <memory>

#include "data/PowerUpType.hpp"

class Entity;

class PowerUpSystem {
public:
    std::vector<PowerUpType> generateOptions(int count = 3);

    void apply(Entity *player, PowerUpType type);
};
