#include "systems/BossBehaviorSystem.hpp"

#include "components/Boss.hpp"
#include "components/Position.hpp"
#include "components/Velocity.hpp"
#include "components/Health.hpp"
#include "components/BossPatternData.hpp"
#include "components/Asteroid.hpp"
#include "components/Shape.hpp"
#include "components/TrapLaser.hpp"

#include "config/BossConfig.hpp"
#include "config/AsteroidConfig.hpp"

#include "data/AsteroidTemplate.hpp"
#include "systems/AsteroidSpawnSystem.hpp"

#include <cmath>
#include <iostream>
#include <random>
#include <utility>

#include "components/Acceleration.hpp"
#include "components/BossSpawnedAsteroid.hpp"
#include "components/Textured.hpp"
#include "config/PlayerConfig.hpp"
#include "config/SetupConfig.hpp"
#include "config/TrapLaserConfig.hpp"

static std::mt19937 rng{std::random_device{}()};

static float length(const sf::Vector2f &v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

BossBehaviorSystem::BossBehaviorSystem() {
    if (!trapAsteroidTexture.loadFromFile(BOSS_ASTEROIDS_TO_BLOW_TEXTURE_PATH)) {
        exit(1);
    }
}

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
        const sf::Texture *texture = &trapAsteroidTexture;

        auto asteroid = std::make_unique<Entity>();
        asteroid->addComponent(std::make_unique<BossSpawnedAsteroid>()); // маркер астероид босса
        asteroid->addComponent(std::make_unique<Position>(origin.x, origin.y));
        asteroid->addComponent(std::make_unique<Velocity>(
            std::cos(angle) * speed,
            std::sin(angle) * speed
        ));
        asteroid->addComponent(std::make_unique<Acceleration>(0.f, BOSS_ASTEROIDS_TO_BLOW_ACCELERATION));
        asteroid->addComponent(std::make_unique<Asteroid>(AsteroidSize::SMALL));
        asteroid->addComponent(std::make_unique<Health>(1.f));
        asteroid->addComponent(std::make_unique<Shape>(
            MEDIUM_ASTEROID_RADIUS,
            MEDIUM_ASTEROID_RADIUS,
            color
        ));
        if (texture) {
            asteroid->addComponent(std::make_unique<Textured>(*texture));
        }
        asteroids.push_back(std::move(asteroid));
    }
}

void BossBehaviorSystem::spawnTrapLasers(
    std::vector<std::unique_ptr<Entity> > &trapLasers,
    const sf::Vector2f &playerPosition
) {
    std::uniform_int_distribution<int> countDist(TRAP_LASER_COUNT_MIN, TRAP_LASER_COUNT_MAX);
    int laserCount = countDist(rng);

    std::uniform_real_distribution<float> xDist(TRAP_LASER_MARGIN, WINDOW_WIDTH - TRAP_LASER_MARGIN);
    std::uniform_real_distribution<float> angleDist(-TRAP_LASER_ANGLE, TRAP_LASER_ANGLE);

    // Лямбда создания лазера
    auto createLaser = [&](sf::Vector2f position, float angle) {
        auto laser = std::make_unique<Entity>();

        // TrapLaser компонент
        auto trapComp = std::make_unique<TrapLaser>();
        trapComp->position = position;
        trapComp->totalLifetime = BOSS_LASER_DURATION;
        trapComp->activeDelay = BOSS_LASER_WARNING_TIME;
        laser->addComponent(std::move(trapComp));

        // Position компонент
        laser->addComponent(std::make_unique<Position>(position.x, position.y));

        // Renderable компонент
        auto renderable = std::make_unique<Shape>(
            TRAP_LASER_BASE_WIDTH,
            TRAP_LASER_BASE_HEIGHT,
            TRAP_LASER_WARNING_COLOR
        );
        renderable->shape.setOrigin({TRAP_LASER_BASE_X_ORIGIN, TRAP_LASER_BASE_Y_ORIGIN});
        renderable->shape.setRotation(sf::degrees(angle));
        laser->addComponent(std::move(renderable));

        return laser;
    };

    // auto aim
    float playerAngle = angleDist(rng);
    trapLasers.push_back(createLaser(playerPosition, playerAngle));

    // default
    for (int i = 1; i < laserCount; ++i) {
        float x = xDist(rng);
        float angle = angleDist(rng);
        trapLasers.push_back(createLaser({x, 0.f}, angle));
    }
}

bool BossBehaviorSystem::handleEntryPhase(
    BossPatternData &pattern,
    Position &pos,
    Velocity &vel,
    float dt
) {
    if (pattern.hasEntered) {
        return true;
    }

    if (pos.value.y < BOSS_ENTRY_Y_POS) {
        vel.value = {0.f, BOSS_ENTRY_Y_SPEED};
        return false;
    }

    // Зафиксировать Y
    pos.value.y = BOSS_ENTRY_Y_POS;
    vel.value.y = 0.f;

    // Выравнивание по X
    float dx = WINDOW_CENTER_X - pos.value.x;
    if (std::abs(dx) <= BOSS_ENTRY_Y_SPEED * dt) {
        pos.value.x = WINDOW_CENTER_X;
        pattern.hasEntered = true;
        pattern.sineStartTime = pattern.timeSinceSpawn;
        vel.value.x = 0.f;
        return true;
    }

    vel.value.x = (dx > 0 ? BOSS_ENTRY_Y_SPEED : -BOSS_ENTRY_Y_SPEED);
    return false;
}

void BossBehaviorSystem::initializeMovementZone(
    BossPatternData &pattern,
    const sf::Vector2f &center,
    const sf::Vector2f &size
) {
    // сброс поведения
    pattern.movementZone.position = {center.x - size.x, center.y - size.y};
    pattern.movementZone.size = {size.x * DOUBLE_FACTOR, size.y * DOUBLE_FACTOR};
    pattern.targetPos = center;
    pattern.targetReached = true;
    pattern.timeSinceLastTargetChange = 0.f;
    pattern.approachVelocity = {0.f, 0.f};
}

void BossBehaviorSystem::moveWithinZone(
    BossPatternData &pattern,
    Position &pos,
    Velocity &vel,
    float dt,
    float targetChangeInterval,
    float stiffness,
    float damping,
    float arriveDistance,
    float arriveSpeed
) {
    // Инициализация зоны (если не задана)
    if (pattern.movementZone.size.x == 0.f && pattern.movementZone.size.y == 0.f) {
        return;
    }

    // Смена цели по времени или при достижении
    pattern.timeSinceLastTargetChange += dt;
    if (pattern.targetReached || pattern.timeSinceLastTargetChange >= targetChangeInterval) {
        std::uniform_real_distribution<float> xDist(
            pattern.movementZone.position.x,
            pattern.movementZone.position.x + pattern.movementZone.size.x
        );
        std::uniform_real_distribution<float> yDist(
            pattern.movementZone.position.y,
            pattern.movementZone.position.y + pattern.movementZone.size.y
        );

        pattern.targetPos = {xDist(rng), yDist(rng)};
        pattern.targetReached = false;
        pattern.timeSinceLastTargetChange = 0.f;
    }

    // Пружинное притяжение к цели
    sf::Vector2f toTarget = pattern.targetPos - pos.value;
    pattern.approachVelocity += toTarget * stiffness * dt;
    pattern.approachVelocity *= (1.0f - damping * dt);
    vel.value = pattern.approachVelocity;

    // Проверка достижения
    if (length(toTarget) < arriveDistance && length(pattern.approachVelocity) < arriveSpeed) {
        pattern.targetReached = true;
    }
}

void BossBehaviorSystem::update(
    std::unique_ptr<Entity> &boss,
    const sf::Vector2f &playerPos,
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
            if (pattern->movementZone.position.x == 0.f) {
                initializeMovementZone(
                    *pattern,
                    {WINDOW_CENTER_X, BOSS_ENTRY_Y_POS},
                    {BOSS_ZONE_WIDTH, BOSS_ZONE_HEIGHT}
                );
            }

            // Движение внутри зоны
            moveWithinZone(
                *pattern,
                *pos,
                *vel,
                dt
            );

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
                    trapLasers,
                    playerPos
                );
            }

            break;
        }

        case BossType::MEDIUM: {
            if (pattern->movementZone.position.x == 0.f) {
                initializeMovementZone(
                    *pattern,
                    {WINDOW_CENTER_X, BOSS_ENTRY_Y_POS},
                    {BOSS_ZONE_WIDTH, BOSS_ZONE_HEIGHT}
                );
            }

            // Движение внутри зоны
            moveWithinZone(
                *pattern,
                *pos,
                *vel,
                dt
            );

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
            if (pattern->movementZone.position.x == 0.f) {
                initializeMovementZone(
                    *pattern,
                    {WINDOW_CENTER_X, BOSS_ENTRY_Y_POS},
                    {BOSS_ZONE_WIDTH, BOSS_ZONE_HEIGHT}
                );
            }

            // Движение внутри зоны
            moveWithinZone(
                *pattern,
                *pos,
                *vel,
                dt
            );

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
                    trapLasers,
                    playerPos
                );
            }
            break;
        }
    }
}
