#pragma once

#include <vector>
#include <memory>

class Entity;

class TextureAnimationSystem {
public:
    // для наборов
    void update(
        std::vector<std::unique_ptr<Entity> > &entities,
        float deltaTime
    );

    // для единичного
    void update(
        Entity &entity,
        float deltaTime
    );
};
