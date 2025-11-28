#include "systems/InvincibilitySystem.hpp"

#include "components/Invincibility.hpp"
#include "entities/Entity.hpp"

void InvincibilitySystem::update(
    Entity *entity,
    float deltaTime
) {
    if (!entity) {
        return;
    }

    if (auto *invincibility = entity->getComponent<Invincibility>()) {
        invincibility->update(deltaTime);
    }
}
