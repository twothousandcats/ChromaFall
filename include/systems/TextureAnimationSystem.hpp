#pragma once

#include <vector>
#include <memory>

class Entity;

class TextureAnimationSystem {
public:
    // набор
    void update(
        std::vector<std::unique_ptr<Entity> > &entities,
        float deltaTime
    );

    // ед
    void update(
        Entity &entity,
        float deltaTime
    );
};
