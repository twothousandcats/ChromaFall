#pragma once

#include <memory>
#include <vector>

#include "components/Boss.hpp"
#include "entities/Entity.hpp"

class BossSystem {
public:
    static std::unique_ptr<Entity> spawnBoss(
        BossType type
    );
};