#pragma once

class Entity;

class InvincibilitySystem {
public:
    static void update(
        Entity *entity,
        float dt
    );
};
