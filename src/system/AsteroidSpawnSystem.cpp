#include "systems/AsteroidSpawnSystem.hpp"

#include <iostream>

#include "components/Shape.hpp"
#include "components/Position.hpp"
#include "components/Velocity.hpp"
#include "components/Acceleration.hpp"
#include "components/Health.hpp"
#include "components/Textured.hpp"
#include "config/AsteroidConfig.hpp"
#include "config/GameConfig.hpp"
#include "config/SetupConfig.hpp"
#include "data/AsteroidTemplate.hpp"
#include "entities/Entity.hpp"

AsteroidSpawnSystem::AsteroidSpawnSystem()
    : randomEngine(std::random_device{}()),
      asteroidTypeDist(0, 2),
      asteroidIntervalDist(MIN_GEN_INTERVAL, MAX_GEN_INTERVAL) {
    // TODO: стоит ли вообще дропать?
    if (!smallAsteroidTexture.loadFromFile(ASTEROID_TEXTURE_PATH_SMALL)) {
        exit(1);
    }
    if (!mediumAsteroidTexture.loadFromFile(ASTEROID_TEXTURE_PATH_MEDIUM)) {
        exit(1);
    }
    if (!largeAsteroidTexture.loadFromFile(ASTEROID_TEXTURE_PATH_LARGE)) {
        exit(1);
    }
}

float AsteroidSpawnSystem::getSpawnIntervalForWave(const int currentWave) {
    float interval = BASE_SPAWN_INTERVAL;
    for (int i = 1; i < currentWave; ++i) {
        interval *= INTERVAL_REDUCTION_FACTOR;
    }

    return std::max(0.3f, interval);
}

std::unique_ptr<Entity> AsteroidSpawnSystem::createAsteroid(
    float x,
    float y,
    float xVel,
    AsteroidSize size
) {
    auto &templates = getAsteroidTemplates();
    const AsteroidTemplate *currentTemplate = nullptr;
    for (const auto &t: templates) {
        if (t.size == size) {
            currentTemplate = &t;
            break;
        }
    }

    if (!currentTemplate) {
        return nullptr;
    }

    const sf::Texture *texture = nullptr;
    switch (size) {
        case AsteroidSize::LARGE: texture = &largeAsteroidTexture;
            break;
        case AsteroidSize::MEDIUM: texture = &mediumAsteroidTexture;
            break;
        case AsteroidSize::SMALL: texture = &smallAsteroidTexture;
            break;
    }


    if (texture) {
        std::cout << "yes" << std::endl;
    } else {
        std::cout << "no" << std::endl;
    }

    auto asteroid = std::make_unique<Entity>();
    asteroid->addComponent(std::make_unique<Position>(x, y));
    asteroid->addComponent(std::make_unique<Velocity>(xVel, 0.f));
    asteroid->addComponent(std::make_unique<Acceleration>(0.f, ASTEROID_LINEAR_ACCELERATION));
    asteroid->addComponent(std::make_unique<Asteroid>(size));
    asteroid->addComponent(std::make_unique<Health>(currentTemplate->health));
    asteroid->addComponent(std::make_unique<Shape>(
        currentTemplate->radius,
        currentTemplate->radius,
        currentTemplate->color
    ));
    if (texture) {
        asteroid->addComponent(std::make_unique<Textured>(*texture));
    }
    return asteroid;
}

void AsteroidSpawnSystem::spawnChildAsteroids(
    std::vector<std::unique_ptr<Entity> > &asteroids,
    const sf::Vector2f &parentPos,
    const sf::Vector2f &parentVel,
    const AsteroidSize parentSize
) {
    if (parentSize == AsteroidSize::SMALL) {
        return;
    }

    const AsteroidSize childSize = (parentSize == AsteroidSize::LARGE)
                                       ? AsteroidSize::MEDIUM
                                       : AsteroidSize::SMALL;

    auto &templates = getAsteroidTemplates();
    const AsteroidTemplate *childTemplate = nullptr;
    for (const auto &t: templates) {
        if (t.size == childSize) {
            childTemplate = &t;
            break;
        }
    }
    if (!childTemplate) {
        return;
    }

    std::uniform_real_distribution<float> angleDist(-ASTEROID_ANGLE_SPREAD_FACTOR, ASTEROID_ANGLE_SPREAD_FACTOR);
    std::uniform_real_distribution<float> speedMult(0.8f, 1.2f);

    for (int i = 0; i < 2; ++i) {
        const float angleOffset = angleDist(randomEngine);
        const float speedFactor = speedMult(randomEngine);

        sf::Vector2f childVel = parentVel;
        const float len = std::sqrt(childVel.x * childVel.x + childVel.y * childVel.y);
        const float baseAngle = std::atan2(childVel.y, childVel.x);
        const float newAngle = baseAngle + angleOffset;
        childVel.x = std::cos(newAngle) * len * speedFactor;
        childVel.y = std::sin(newAngle) * len * speedFactor;

        auto child = createAsteroid(parentPos.x, parentPos.y, childVel.x, childSize);
        if (child) {
            asteroids.push_back(std::move(child));
        }
    }
}

void AsteroidSpawnSystem::update(
    std::vector<std::unique_ptr<Entity> > &asteroids,
    const int currentWave
) {
    spawnInterval = getSpawnIntervalForWave(currentWave);
    if (spawnClock.getElapsedTime().asSeconds() >= spawnInterval) {
        auto &templates = getAsteroidTemplates();
        const int index = asteroidTypeDist(randomEngine) % templates.size();
        const auto &currentTemplate = templates[index];

        std::uniform_real_distribution<float> xDist(
            ASTEROID_EDGE_X + currentTemplate.radius,
            WINDOW_WIDTH - currentTemplate.radius
        );
        std::uniform_real_distribution<float> xVelocityDist(
            -ASTEROID_EDGE_VELOCITY,
            ASTEROID_EDGE_VELOCITY
        );

        const float x = xDist(randomEngine);
        const float xVel = xVelocityDist(randomEngine);

        auto asteroid = createAsteroid(x, -currentTemplate.radius, xVel, currentTemplate.size);
        if (asteroid) {
            asteroids.push_back(std::move(asteroid));
        }

        spawnClock.restart();
    }
}
