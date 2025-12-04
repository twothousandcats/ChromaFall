#pragma once

#include <memory>
#include <vector>

#include "entities/Entity.hpp"

class BossBehaviorSystem {
public:
    static void update(
        std::unique_ptr<Entity> &boss,
        std::vector<std::unique_ptr<Entity> > &asteroids, // для MEDIUM/HARD
        std::vector<std::unique_ptr<Entity> > &trapLasers, // для HARD
        float deltaTime
    );
};
