#include "iostream"

#include "systems/GameSession.hpp"

#include "components/Acceleration.hpp"
#include "components/Health.hpp"
#include "components/Invincibility.hpp"
#include "components/Position.hpp"
#include "components/Renderable.hpp"
#include "components/Velocity.hpp"
#include "config/AsteroidConfig.hpp"
#include "config/GameConfig.hpp"
#include "config/PlayerConfig.hpp"
#include "config/SetupConfig.hpp"
#include "data/AsteroidTemplate.hpp"
#include "systems/PlayerControlSystem.hpp"
#include "systems/RenderSystem.hpp"

GameSession::GameSession(sf::RenderWindow &window) : window(window) {
    reset();

    std::random_device rd;
    randomEngine.seed(rd());
    asteroidTypeDist = std::uniform_int_distribution<int>(0, 2);
    asteroidIntervalDist = std::uniform_real_distribution<float>(MIN_GEN_INTERVAL, MAX_GEN_INTERVAL);
}

void GameSession::reset() {
    player = std::make_unique<Entity>();
    player->addComponent(std::make_unique<Position>());
    player->addComponent(std::make_unique<Renderable>());
    player->addComponent(std::make_unique<Health>(3.f));
    player->addComponent(std::make_unique<Invincibility>());

    bullets.clear();
    asteroids.clear();
    asteroidSpawnClock.restart();
    gameOver = false;
}

void GameSession::update(
    const float dt
) {
    if (gameOver) {
        return;
    }

    // таймер неуязвимости
    if (auto *invisibility = player->getComponent<Invincibility>()) {
        invisibility->update(dt);
    }
    PlayerControlSystem::update(*player, window);
    auto *playerPosition = player->getComponent<Position>();
    if (playerPosition) {
        shootSystem.update(
            bullets,
            window,
            playerPosition->value,
            bulletsCount,
            spreadAngle
        );
    }

    movementSystem.update(bullets, dt);
    movementSystem.update(asteroids, dt);

    if (!gameOver && waveSystem.shouldSpawnAsteroids()) {
        if (asteroidSpawnClock.getElapsedTime().asSeconds() >= currentSpawnInterval) {
            spawnAsteroid();
            currentSpawnInterval = asteroidIntervalDist(randomEngine);
            asteroidSpawnClock.restart();
        }
    }

    auto [isPlayerHit, isPlayerDied, destroyedAsteroidsCount, asteroidsToSplit] = CollisionSystem::update(
        *player, bullets, asteroids);
    for (const auto [pos, vel, size]: asteroidsToSplit) {
        spawnChildAsteroid(pos, vel, size);
    }

    waveSystem.update(destroyedAsteroidsCount, dt);

    cleanEntities();

    if (isPlayerDied) {
        gameOver = true;
    }
}

float GameSession::getPlayerHp() const {
    if (const auto *playerHp = player->getComponent<Health>()) {
        return playerHp->value;
    }

    return 0.f;
}

void GameSession::spawnAsteroid() {
    auto &templates = getAsteroidTemplates();
    int index = asteroidTypeDist(randomEngine) % static_cast<int>(templates.size());
    const auto &currentTemplate = templates[index];

    auto asteroid = std::make_unique<Entity>();
    // определяем случайный х
    std::uniform_real_distribution<float> xDist(
        ASTEROID_EDGE_X + currentTemplate.radius,
        WINDOW_WIDTH - currentTemplate.radius
    );
    float x = xDist(randomEngine);
    std::uniform_real_distribution<float> xVelocityDist(-ASTEROID_EDGE_VELOCITY, ASTEROID_EDGE_VELOCITY);
    float xVel = xVelocityDist(randomEngine);

    asteroid->addComponent(std::make_unique<Position>(x, -currentTemplate.radius));
    asteroid->addComponent(std::make_unique<Velocity>(xVel, 0));
    asteroid->addComponent(std::make_unique<Acceleration>(0.f, ASTEROID_LINEAR_ACCELERATION));
    asteroid->addComponent(std::make_unique<Asteroid>(currentTemplate.size));
    asteroid->addComponent(std::make_unique<Health>(currentTemplate.health));
    asteroid->addComponent(std::make_unique<Renderable>(
        currentTemplate.radius,
        currentTemplate.radius,
        currentTemplate.color
    ));

    asteroids.push_back(std::move(asteroid));
}

void GameSession::spawnChildAsteroid(
    const sf::Vector2f &parentPos,
    const sf::Vector2f &parentVel,
    const AsteroidSize parentSize
) {
    if (parentSize == AsteroidSize::SMALL) {
        return;
    }

    const AsteroidSize childSize = parentSize == AsteroidSize::LARGE
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

    // генерация разброса. Мб лучше задать дефолтные значения
    std::uniform_real_distribution<float> angleDist(-0.5f, 0.5f);
    std::uniform_real_distribution<float> speedMult(0.8f, 1.2f);

    for (int i = 0; i < 2; ++i) {
        float angleOffset = angleDist(randomEngine);
        float speedFactor = speedMult(randomEngine);

        sf::Vector2f childVel = parentVel;
        // Поворачиваем вектор скорости
        float len = std::sqrt(childVel.x * childVel.x + childVel.y * childVel.y);
        float baseAngle = std::atan2(childVel.y, childVel.x);
        float newAngle = baseAngle + angleOffset;
        childVel.x = std::cos(newAngle) * len * speedFactor;
        childVel.y = std::sin(newAngle) * len * speedFactor;

        auto child = std::make_unique<Entity>();
        child->addComponent(std::make_unique<Position>(parentPos.x, parentPos.y));
        child->addComponent(std::make_unique<Velocity>(childVel.x, childVel.y));
        child->addComponent(std::make_unique<Acceleration>(0.f, ASTEROID_LINEAR_ACCELERATION));
        child->addComponent(std::make_unique<Asteroid>(childSize));
        child->addComponent(std::make_unique<Health>(childTemplate->health));
        child->addComponent(std::make_unique<Renderable>(
            childTemplate->radius,
            childTemplate->radius,
            childTemplate->color
        ));
        asteroids.push_back(std::move(child));
    }
}

void GameSession::setupEntities(
    sf::RenderWindow &window,
    const std::vector<std::unique_ptr<Entity> > &entities
) {
    if (!entities.empty()) {
        std::vector<Entity *> ptrs;
        ptrs.reserve(entities.size());
        for (auto &entity: entities) {
            ptrs.push_back(entity.get());
        }
        RenderSystem::render(window, ptrs);
    }
}

void GameSession::cleanEntities() {
    // bullets
    for (auto it = bullets.begin(); it != bullets.end();) {
        auto *pos = (*it)->getComponent<Position>();
        if (!pos || pos->value.y < -PLAYER_SIDE) {
            it = bullets.erase(it);
        } else {
            ++it;
        }
    }

    // asteroids
    for (auto it = asteroids.begin(); it != asteroids.end();) {
        auto *pos = (*it)->getComponent<Position>();
        if (!pos || pos->value.y > WINDOW_HEIGHT + LARGE_ASTEROID_RADIUS) {
            it = asteroids.erase(it);
        } else {
            ++it;
        }
    }
}

void GameSession::render(
    sf::RenderWindow &window
) {
    if (player) {
        bool shouldDraw = true;

        if (auto *playerInvisibility = player->getComponent<Invincibility>()) {
            if (playerInvisibility->isActive()) {
                float blinkTimer = gameClock.getElapsedTime().asSeconds();
                shouldDraw = (static_cast<int>(blinkTimer * 5) % 2 == 0);
            }
        }

        if (shouldDraw) {
            RenderSystem::render(window, {player.get()});
        }
    }

    setupEntities(window, bullets);
    setupEntities(window, asteroids);
}
