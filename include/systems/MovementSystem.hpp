#pragma once

#include <vector>

class Entity;

class MovementSystem {
public:
    void update(
        std::vector<std::unique_ptr<Entity> > &entities,
        float deltaTime
    );

    void update(
        Entity &entity,
        float deltaTime
    );
};
