#pragma once

#include <vector>

class Entity;

class EntityCleanupSystem {
public:
    static void cleanupBullets(std::vector<std::unique_ptr<Entity> > &bullets);

    static void cleanupAsteroids(std::vector<std::unique_ptr<Entity> > &asteroids);
};
