#include "iostream"

#include "systems/GameSession.hpp"

#include "components/Acceleration.hpp"
#include "components/Health.hpp"
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

    if (asteroidSpawnClock.getElapsedTime().asSeconds() >= currentSpawnInterval) {
        spawnAsteroid();
        currentSpawnInterval = asteroidIntervalDist(randomEngine);
        asteroidSpawnClock.restart();
    }

    checkCollision();
    cleanEntities();

    if (getPlayerHp() <= 0.f) {
        gameOver = true;
    }
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

bool GameSession::isIntersects(
    const sf::Vector2f &aPos,
    const sf::Vector2f &bPos,
    const sf::Vector2f &aSize,
    const sf::Vector2f &bSize
) {
    const float aLeft = aPos.x - aSize.x / HALF_DIVISOR;
    const float aRight = aPos.x + aSize.x / HALF_DIVISOR;
    const float aTop = aPos.y - aSize.y / HALF_DIVISOR;
    const float aBottom = aPos.y + aSize.y / HALF_DIVISOR;

    const float bLeft = bPos.x - bSize.x / HALF_DIVISOR;
    const float bRight = bPos.x + bSize.x / HALF_DIVISOR;
    const float bTop = bPos.y - bSize.y / HALF_DIVISOR;
    const float bBottom = bPos.y + bSize.y / HALF_DIVISOR;

    return aRight > bLeft
           && aLeft < bRight
           && aBottom > bTop
           && aTop < bBottom;
}

void GameSession::checkCollision() {
    // bullet -> asteroid
    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
        auto *bulletPos = (*bulletIt)->getComponent<Position>();
        auto *bulletRender = (*bulletIt)->getComponent<Renderable>();
        bool bulletHit = false;

        if (bulletPos && bulletRender) {
            sf::Vector2f bulletSize = bulletRender->shape.getSize();

            for (auto asteroidIt = asteroids.begin(); asteroidIt != asteroids.end();) {
                auto *asteroidPos = (*asteroidIt)->getComponent<Position>();
                auto *asteroidRender = (*asteroidIt)->getComponent<Renderable>();
                auto *asteroidHealth = (*asteroidIt)->getComponent<Health>();

                if (asteroidPos && asteroidRender && asteroidHealth) {
                    sf::Vector2f asteroidSize = asteroidRender->shape.getSize();
                    // попадание
                    if (isIntersects(
                        bulletPos->value,
                        asteroidPos->value,
                        bulletSize,
                        asteroidSize
                    )) {
                        asteroidHealth->value -= 1.f;
                        bulletHit = true;

                        if (asteroidHealth->value <= 0.f) {
                            asteroidIt = asteroids.erase(asteroidIt);
                        } else {
                            ++asteroidIt;
                        }
                        break;
                    } else {
                        ++asteroidIt;
                    }
                } else {
                    ++asteroidIt;
                }
            }
        }

        if (bulletHit) {
            bulletIt = bullets.erase(bulletIt);
        } else {
            ++bulletIt;
        }
    }

    // asteroid -> player
    if (player) {
        auto *playerPos = player->getComponent<Position>();
        auto *playerRender = player->getComponent<Renderable>();
        if (playerPos && playerRender) {
            sf::Vector2f playerSize = playerRender->shape.getSize();
            auto *playerHealth = player->getComponent<Health>();
            for (auto asteroidIt = asteroids.begin(); asteroidIt != asteroids.end();) {
                auto *asteroidPos = (*asteroidIt)->getComponent<Position>();
                auto *asteroidRender = (*asteroidIt)->getComponent<Renderable>();
                if (asteroidPos && asteroidRender) {
                    sf::Vector2f asteroidSize = asteroidRender->shape.getSize();
                    if (isIntersects(
                        playerPos->value,
                        asteroidPos->value,
                        playerSize,
                        asteroidSize
                    )) {
                        asteroidIt = asteroids.erase(asteroidIt);
                        playerHealth->value -= 1.f;
                        std::cout << "player hit!" << getPlayerHp() << std::endl;
                    } else {
                        ++asteroidIt;
                    }
                } else {
                    ++asteroidIt;
                }
            }
        }
    }
}

float GameSession::getPlayerHp() const {
    if (player) {
        if (const auto *hp = player->getComponent<Health>()) {
            return hp->value;
        }
    }

    return 0.f;
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

    // asteroisd
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
        RenderSystem::render(window, {player.get()});
    }

    setupEntities(window, bullets);
    setupEntities(window, asteroids);
}
