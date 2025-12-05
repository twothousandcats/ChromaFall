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
        float offsetAngle = (i - 2) * BOSS_ASTEROIDS_TO_BLOW_OFFSET_FACTOR; // -0.5 .. +0.5 рад
        float angle = -M_PI_2 + offsetAngle;
        float speed = baseSpeed;

        auto asteroid = std::make_unique<Entity>();
        asteroid->addComponent(std::make_unique<Position>(origin.x, origin.y));
        asteroid->addComponent(std::make_unique<Velocity>(
            std::cos(angle) * speed,
            std::sin(angle) * speed
        ));
        asteroid->addComponent(std::make_unique<Acceleration>(0.f, BOSS_ASTEROIDS_TO_BLOW_ACCELERATION));
        asteroid->addComponent(std::make_unique<Asteroid>(AsteroidSize::SMALL));
        asteroid->addComponent(std::make_unique<Health>(1.f));
        asteroid->addComponent(std::make_unique<Renderable>(
            MEDIUM_ASTEROID_RADIUS,
            MEDIUM_ASTEROID_RADIUS,
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
    std::uniform_real_distribution<float> angleDist(-TRAP_LASER_ANGLE, TRAP_LASER_ANGLE);

    for (int i = 0; i < laserCount; ++i) {
        float laserX = xDist(rng);
        float angle = angleDist(rng);

        auto laser = std::make_unique<Entity>();
        auto trapComp = std::make_unique<TrapLaser>();
        trapComp->position = {laserX, 0.f};
        trapComp->totalLifetime = BOSS_LASER_DURATION;
        trapComp->activeDelay = BOSS_LASER_WARNING_TIME;
        laser->addComponent(std::move(trapComp));

        // верхняя точка лазера
        laser->addComponent(std::make_unique<Position>(laserX, 0.f));

        auto renderable = std::make_unique<Renderable>(
            TRAP_LASER_BASE_WIDTH,
            TRAP_LASER_BASE_HEIGHT,
            TRAP_LASER_WARNING_COLOR
        );

        // Origin верх центр
        renderable->shape.setOrigin({TRAP_LASER_BASE_X_ORIGIN, TRAP_LASER_BASE_Y_ORIGIN});
        renderable->shape.setRotation(sf::degrees(angle));
        laser->addComponent(std::move(renderable));

        trapLasers.push_back(std::move(laser));
    }
}

bool BossBehaviorSystem::handleEntryPhase(
    BossPatternData &pattern,
    Position &pos,
    Velocity &vel,
    float dt
) {
    // Спуск по Y
    if (pos.value.y < BOSS_ENTRY_Y_POS) {
        vel.value = {0.f, BOSS_ENTRY_Y_SPEED};
        return false; // вход ещё не завершён
    }

    // фикс по Y
    pos.value.y = BOSS_ENTRY_Y_POS;
    vel.value.y = 0.f;

    // выравнивание по Х
    if (!pattern.hasEntered) {
        float dx = WINDOW_CENTER_X - pos.value.x;
        float distance = std::abs(dx);

        if (distance <= BOSS_ENTRY_Y_SPEED * dt) {
            // Достиг центра
            pos.value.x = WINDOW_CENTER_X;
            pattern.hasEntered = true;
            pattern.sineStartTime = pattern.timeSinceSpawn;
        } else {
            vel.value.x = (dx > 0 ? BOSS_ENTRY_Y_SPEED : -BOSS_ENTRY_Y_SPEED);
            pos.value.x += vel.value.x * dt;
        }
        return false; // всё ещё в фазе входа
    }

    // Сброс
    vel.value.x = 0.f;
    return true;
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

    // фаза подготовки
    if (!handleEntryPhase(*pattern, *pos, *vel, dt)) {
        return;
    }

    // Поведение по типу
    switch (bossComp->type) {
        case BossType::EASY: {
            float sineTime = pattern->timeSinceSpawn - pattern->sineStartTime;
            pos->value.x = WINDOW_CENTER_X + BOSS_AMPLITUDE * std::sin(BOSS_FREQUENCY * sineTime);

            if (pattern->timeSinceSpawn - pattern->lastAttackTime >= BOSS_ATTACK_INTERVAL) {
                pattern->lastAttackTime = pattern->timeSinceSpawn;
                //     pattern->lastAttackTime = pattern->timeSinceSpawn;
                //     spawnUpwardAsteroids(
                //         asteroids,
                //         pos->value,
                //         BOSS_ASTEROIDS_TO_BLOW_COUNT,
                //         BOSS_ASTEROIDS_TO_BLOW_SPEED,
                //         BOSS_ASTEROIDS_TO_BLOW_COLOR
                //     );
                spawnTrapLasers(
                    trapLasers
                );
            }
            break;
        }

        case BossType::MEDIUM: {
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
