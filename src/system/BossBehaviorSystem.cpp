#include "systems/BossBehaviorSystem.hpp"

#include "components/Boss.hpp"
#include "components/Position.hpp"
#include "components/Velocity.hpp"
#include "components/Health.hpp"
#include "components/BossPatternData.hpp"
#include "components/Asteroid.hpp"
#include "components/Renderable.hpp"
#include "components/TrapLaser.hpp"

#include "config/BossConfig.hpp"
#include "config/AsteroidConfig.hpp"

#include "data/AsteroidTemplate.hpp"
#include "systems/AsteroidSpawnSystem.hpp"

#include <cmath>
#include <iostream>
#include <random>

#include "components/Acceleration.hpp"
#include "config/SetupConfig.hpp"
#include "config/TrapLaserConfig.hpp"

static std::mt19937 rng{std::random_device{}()};

// спавн как есть, было бы правильнее привязть к системе спавна
void BossBehaviorSystem::spawnUpwardAsteroids(
    std::vector<std::unique_ptr<Entity> > &asteroids,
    const sf::Vector2f &origin,
    int count,
    float baseSpeed,
    sf::Color color
) {
    for (int i = 0; i < count; ++i) {
        float offsetAngle = (i - 2) * 0.25f; // -0.5 .. +0.5 рад
        float angle = -M_PI_2 + offsetAngle;
        float speed = baseSpeed;

        auto asteroid = std::make_unique<Entity>();
        asteroid->addComponent(std::make_unique<Position>(origin.x, origin.y));
        asteroid->addComponent(std::make_unique<Velocity>(
            std::cos(angle) * speed,
            std::sin(angle) * speed
        ));
        asteroid->addComponent(std::make_unique<Acceleration>(0.f, ASTEROID_LINEAR_ACCELERATION));
        asteroid->addComponent(std::make_unique<Asteroid>(AsteroidSize::SMALL));
        asteroid->addComponent(std::make_unique<Health>(1.f));
        asteroid->addComponent(std::make_unique<Renderable>(
            SMALL_ASTEROID_RADIUS * 2,
            SMALL_ASTEROID_RADIUS * 2,
            color
        ));
        asteroids.push_back(std::move(asteroid));
    }
}

void BossBehaviorSystem::spawnTrapLasers(
    std::vector<std::unique_ptr<Entity> > &trapLasers
) {
    std::uniform_int_distribution<int> countDist(TRAP_LASER_COUNT_MIN, TRAP_LASER_COUNT_MAX);
    int laserCount = countDist(rng);

    const float margin = TRAP_LASER_MARGIN;
    const float minX = margin;
    const float maxX = WINDOW_WIDTH - margin;
    std::uniform_real_distribution<float> xDist(minX, maxX);
    std::uniform_real_distribution<float> angleDist(-60.f, 60.f);

    for (int i = 0; i < laserCount; ++i) {
        float laserX = xDist(rng);
        float angle = angleDist(rng);

        auto laser = std::make_unique<Entity>();
        laser->addComponent(std::make_unique<TrapLaser>());
        auto *laserComp = laser->getComponent<TrapLaser>();
        laserComp->position = {laserX, 0.f};
        laserComp->lifetime = BOSS_LASER_DURATION;
        laserComp->activeDuration = BOSS_LASER_ACTIVE;
        laserComp->isActive = true;

        // верхняя точка лазера
        laser->addComponent(std::make_unique<Position>(laserX, 0.f));

        auto renderable = std::make_unique<Renderable>(
            TRAP_LASER_BASE_WIDTH,
            WINDOW_HEIGHT * 1.1f,
            TRAP_LASER_BASE_COLOR
        );
        // Origin верх центр
        renderable->shape.setOrigin({TRAP_LASER_BASE_WIDTH / 2.f, 0.f});
        renderable->shape.setRotation(sf::degrees(angle));

        laser->addComponent(std::move(renderable));
        trapLasers.push_back(std::move(laser));
    }
}

void BossBehaviorSystem::update(
    std::unique_ptr<Entity> &boss,
    std::vector<std::unique_ptr<Entity> > &asteroids,
    std::vector<std::unique_ptr<Entity> > &trapLasers,
    float dt
) {
    if (!boss) {
        return;
    }

    auto *bossComp = boss->getComponent<Boss>();
    auto *pos = boss->getComponent<Position>();
    auto *vel = boss->getComponent<Velocity>();
    auto *pattern = boss->getComponent<BossPatternData>();

    if (!bossComp || !pos || !vel || !pattern) {
        return;
    }

    pattern->timeSinceSpawn += dt;

    // переход до точки
    const float entryY = 100.f;
    // std::cout << "Входная точка: " + std::to_string(entryY) << std::endl;
    // std::cout << "Текущая точка: " + std::to_string(pos->value.y) << std::endl;
    if (pos->value.y < entryY) {
        vel->value = {0.f, 80.f}; // летит вниз
        return;
    }

    vel->value = {0.f, 0.f};
    pos->value.y = entryY;

    // Поведение по типу
    switch (bossComp->type) {
        case BossType::EASY: {
            // поведение легкого типа
            pos->value.x = WINDOW_WIDTH / 2.f + BOSS_AMPLITUDE * std::sin(BOSS_FREQUENCY * pattern->timeSinceSpawn);
            break;
        }

        case BossType::MEDIUM: {
            // поведение усложненного типа
            pos->value.x = WINDOW_CENTER_X;

            if (pattern->timeSinceSpawn - pattern->lastAttackTime >= BOSS_ATTACK_INTERVAL) {
                pattern->lastAttackTime = pattern->timeSinceSpawn;
                spawnUpwardAsteroids(
                    asteroids,
                    pos->value,
                    BOSS_ASTEROIDS_TO_BLOW_COUNT,
                    BOSS_ASTEROIDS_TO_BLOW_SPEED,
                    BOSS_ASTEROIDS_TO_BLOW_COLOR
                );
            }
            break;
        }

        case BossType::HARD: {
            // поведение сложного типа
            pos->value.x = WINDOW_CENTER_X;

            if (pattern->timeSinceSpawn - pattern->lastAttackTime >= BOSS_ATTACK_INTERVAL) {
                pattern->lastAttackTime = pattern->timeSinceSpawn;
                spawnUpwardAsteroids(
                    asteroids,
                    pos->value,
                    BOSS_ASTEROIDS_TO_BLOW_COUNT,
                    BOSS_ASTEROIDS_TO_BLOW_SPEED * 2.f,
                    BOSS_ASTEROIDS_TO_BLOW_COLOR
                );
                spawnTrapLasers(
                    trapLasers
                );
            }
            break;
        }
    }
}
