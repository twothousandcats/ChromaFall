#include "systems/MovementSystem.hpp"

#include "components/Acceleration.hpp"
#include "components/Position.hpp"
#include "components/Velocity.hpp"
#include "entities/Entity.hpp"

void MovementSystem::update(
    std::vector<std::unique_ptr<Entity> > &entities,
    const float deltaTime
) {
    for (auto &entity: entities) {
        auto *position = entity->getComponent<Position>();
        auto *velocity = entity->getComponent<Velocity>();
        if (!position || !velocity) {
            continue;
        }

        // обновляем скорость
        if (auto *acceleration = entity->getComponent<Acceleration>()) {
            velocity->value += acceleration->value * deltaTime;
        }

        position->value += velocity->value * deltaTime;
    }
}
