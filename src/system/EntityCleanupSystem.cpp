#include "systems/EntityCleanupSystem.hpp"

#include <iostream>

#include "components/Health.hpp"
#include "components/Position.hpp"
#include "config/AsteroidConfig.hpp"
#include "config/BulletConfig.hpp"
#include "config/SetupConfig.hpp"
#include "entities/Entity.hpp"

void EntityCleanupSystem::cleanupBullets(std::vector<std::unique_ptr<Entity> > &bullets) {
    for (auto it = bullets.begin(); it != bullets.end();) {
        auto *pos = (*it)->getComponent<Position>();
        if (!pos || pos->value.y < -BULLET_HEIGHT) {
            it = bullets.erase(it);
        } else {
            ++it;
        }
    }
}

void EntityCleanupSystem::cleanupAsteroids(std::vector<std::unique_ptr<Entity> > &asteroids) {
    for (auto it = asteroids.begin(); it != asteroids.end();) {
        auto *pos = (*it)->getComponent<Position>();
        std::cout << "" << std::endl;
        if (!pos || pos->value.y > WINDOW_HEIGHT + LARGE_ASTEROID_RADIUS) {
            it = asteroids.erase(it);
        } else {
            ++it;
        }
    }
}


void EntityCleanupSystem::cleanupByHealth(std::vector<std::unique_ptr<Entity>>& entities) {
    for (auto it = entities.begin(); it != entities.end();) {
        auto* health = (*it)->getComponent<Health>();
        if (health && health->value <= 0.f) {
            it = entities.erase(it);
        } else {
            ++it;
        }
    }
};
