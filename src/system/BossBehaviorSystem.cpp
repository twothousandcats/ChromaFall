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

// Глобальный генератор (можно вынести, но для простоты — локальный)
static std::mt19937 rng{std::random_device{}()};

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

    // === Этап 1: вход с верхней границы ===
    const float entryY = 100.f;
    std::cout << "Входная точка: " + std::to_string(entryY) << std::endl;
    std::cout << "Текущая позиция точка: " + std::to_string(pos->value.y) << std::endl;
    if (pos->value.y < entryY) {
        vel->value = {0.f, 80.f}; // летит вниз
        return;
    }

    vel->value = {0.f, 0.f};
    pos->value.y = entryY;

    // === Этап 2: поведение по типу ===
    switch (bossComp->type) {
        case BossType::EASY: {
            // Синусоида по X, астероиды продолжают падать
            const float amplitude = 180.f;
            const float frequency = 0.6f;
            pos->value.x = WINDOW_WIDTH / 2.f + amplitude * std::sin(frequency * pattern->timeSinceSpawn);
            break;
        }

        case BossType::MEDIUM: {
            // Фиксированная позиция + извержение астероидов вверх
            pos->value.x = WINDOW_WIDTH / 2.f;

            if (pattern->timeSinceSpawn - pattern->lastAttackTime >= BOSS_ATTACK_INTERVAL) {
                pattern->lastAttackTime = pattern->timeSinceSpawn;

                // Спавн 5 мини-астероидов вверх
                for (int i = 0; i < 25; ++i) {
                    float offsetAngle = (i - 2) * 0.25f; // -0.5 .. +0.5 радиан
                    float angle = -M_PI_2 + offsetAngle; // направление вверх
                    float speed = 120.f;

                    auto asteroid = std::make_unique<Entity>();
                    asteroid->addComponent(std::make_unique<Position>(pos->value.x, pos->value.y));
                    asteroid->addComponent(std::make_unique<Velocity>(
                        std::cos(angle) * speed,
                        std::sin(angle) * speed
                    ));
                    asteroid->addComponent(std::make_unique<Acceleration>(0.f, ASTEROID_LINEAR_ACCELERATION));
                    asteroid->addComponent(std::make_unique<Asteroid>(AsteroidSize::SMALL));
                    asteroid->addComponent(std::make_unique<Health>(1.f));

                    // Цвет как у босса или особый
                    asteroid->addComponent(std::make_unique<Renderable>(
                        SMALL_ASTEROID_RADIUS * 2,
                        SMALL_ASTEROID_RADIUS * 2,
                        sf::Color::Red
                    ));
                    asteroids.push_back(std::move(asteroid));
                }
            }
            break;
        }

        case BossType::HARD: {
            // Фиксированная позиция + астероиды + лазеры
            pos->value.x = WINDOW_CENTER_X;

            if (pattern->timeSinceSpawn - pattern->lastAttackTime >= BOSS_ATTACK_INTERVAL) {
                pattern->lastAttackTime = pattern->timeSinceSpawn;

                // 1. Астероиды (как у Medium)
                for (int i = 0; i < 25; ++i) {
                    float offsetAngle = (i - 2) * 0.25f;
                    float angle = -M_PI_2 + offsetAngle;
                    float speed = 130.f;

                    auto asteroid = std::make_unique<Entity>();
                    asteroid->addComponent(std::make_unique<Position>(pos->value.x, pos->value.y));
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
                        sf::Color::Magenta
                    ));
                    asteroids.push_back(std::move(asteroid));
                }

                // 2. Лазеры: 3 вертикальных луча
                const float laserOffsets[] = {-150.f, 0.f, 150.f};
                for (float offset: laserOffsets) {
                    float laserX = pos->value.x + offset;
                    // Ограничиваем, чтобы не уходили за экран
                    laserX = std::clamp(laserX, 10.f, WINDOW_WIDTH - 10.f);

                    auto laser = std::make_unique<Entity>();
                    laser->addComponent(std::make_unique<TrapLaser>());
                    auto *laserComp = laser->getComponent<TrapLaser>();
                    laserComp->position = {laserX, 0.f};
                    laserComp->lifetime = BOSS_LASER_DURATION; // например, 2.0f
                    laserComp->activeDuration = BOSS_LASER_ACTIVE; // например, 1.5f
                    laserComp->isActive = true;

                    laser->addComponent(std::make_unique<Position>(
                        laserComp->position.x,
                        laserComp->position.y
                    ));
                    laser->addComponent(std::make_unique<Renderable>(
                        TRAP_LASER_BASE_WIDTH,
                        TRAP_LASER_BASE_HEIGHT,
                        TRAP_LASER_BASE_COLOR
                    ));
                    trapLasers.push_back(std::move(laser));
                }
            }
            break;
        }
    }
}
