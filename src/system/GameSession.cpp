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
}

void GameSession::reset() {
    player = std::make_unique<Entity>();
    player->addComponent(std::make_unique<Position>());
    player->addComponent(std::make_unique<Renderable>());
    player->addComponent(std::make_unique<Health>(3.f));
    player->addComponent(std::make_unique<Invincibility>());

    bullets.clear();
    asteroids.clear();
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
        asteroidSpawnSystem.update(
            asteroids,
            waveSystem.getCurrentWave(),
            dt
        );
    }

    auto [isPlayerHit, isPlayerDied, destroyedAsteroidsCount, asteroidsToSplit] = CollisionSystem::update(
        *player,
        bullets,
        asteroids
    );
    for (const auto [pos, vel, size]: asteroidsToSplit) {
        asteroidSpawnSystem.spawnChildAsteroids(
            asteroids,
            pos,
            vel,
            size
        );
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
