#include "iostream"

#include "systems/GameSession.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/EntityCleanupSystem.hpp"
#include "systems/PlayerControlSystem.hpp"
#include "systems/RenderSystem.hpp"

#include "components/Health.hpp"
#include "components/Invincibility.hpp"
#include "components/Position.hpp"
#include "components/Renderable.hpp"

#include "config/PlayerConfig.hpp"

GameSession::GameSession(sf::RenderWindow &window) : window(window) {
    reset();
}

void GameSession::reset() {
    player = std::make_unique<Entity>();
    player->addComponent(std::make_unique<Position>());
    player->addComponent(std::make_unique<Renderable>());
    player->addComponent(std::make_unique<Health>(PLAYER_BASE_HP));
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
            DEFAULT_BULLETS_COUNT,
            DEFAULT_BULLETS_SPREAD_FACTOR
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

    EntityCleanupSystem::cleanupBullets(bullets);
    EntityCleanupSystem::cleanupAsteroids(asteroids);

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

    RenderSystem::render(window, bullets);
    RenderSystem::render(window, asteroids);
}
