#pragma once
#include <memory>
#include <random>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

#include "data/AsteroidTemplate.hpp"

class Entity;
enum class AsteroidSize;

class AsteroidSpawnSystem {
public:
    AsteroidSpawnSystem();

    void update(
        std::vector<std::unique_ptr<Entity> > &asteroids,
        int currentWave,
        float deltaTime
    );

    void spawnChildAsteroids(
        std::vector<std::unique_ptr<Entity> > &asteroids,
        const sf::Vector2f &parentPos,
        const sf::Vector2f &parentVel,
        AsteroidSize parentSize
    );

private:
    sf::Clock spawnClock;
    float spawnInterval = 1.f;

    std::mt19937 randomEngine;
    std::uniform_int_distribution<int> asteroidTypeDist;
    std::uniform_real_distribution<float> asteroidIntervalDist;

    std::unique_ptr<Entity> createAsteroid(
        float x,
        float y,
        float xVel,
        AsteroidSize size
    );

    static float getSpawnIntervalForWave(int currentWave);
};
