#include "systems/EntityCleanupSystem.hpp"

#include "components/Position.hpp"
#include "config/AsteroidConfig.hpp"
#include "config/PlayerConfig.hpp"
#include "config/SetupConfig.hpp"
#include "entities/Entity.hpp"

void EntityCleanupSystem::cleanupBullets(std::vector<std::unique_ptr<Entity> > &bullets) {
    for (auto it = bullets.begin(); it != bullets.end();) {
        auto *pos = (*it)->getComponent<Position>();
        if (!pos || pos->value.y < -PLAYER_SIDE) {
            it = bullets.erase(it);
        } else {
            ++it;
        }
    }
}

void EntityCleanupSystem::cleanupAsteroids(std::vector<std::unique_ptr<Entity> > &asteroids) {
    for (auto it = asteroids.begin(); it != asteroids.end();) {
        auto *pos = (*it)->getComponent<Position>();
        if (!pos || pos->value.y > WINDOW_HEIGHT + LARGE_ASTEROID_RADIUS) {
            it = asteroids.erase(it);
        } else {
            ++it;
        }
    }
}
