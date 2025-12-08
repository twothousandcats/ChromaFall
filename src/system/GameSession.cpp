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
#include "components/TrapLaser.hpp"

#include "config/PlayerConfig.hpp"
#include "systems/InvincibilitySystem.hpp"

GameSession::GameSession(sf::RenderWindow &window) : window(window) {
    reset();
}

void GameSession::reset() {
    player = std::make_unique<Entity>();
    player->addComponent(std::make_unique<Position>());
    player->addComponent(std::make_unique<Renderable>(PLAYER_SIDE, PLAYER_SIDE, PLAYER_COLOR));
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
    InvincibilitySystem::update(player.get(), dt);
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
    if (boss) {
        movementSystem.update(*boss, dt);
    }

    if (waveSystem.shouldSpawnAsteroids()) {
        asteroidSpawnSystem.update(
            asteroids,
            waveSystem.getCurrentWave(),
            dt
        );
    }

    if (waveSystem.isBossPhase() && playerPosition) {
        if (!boss) {
            boss = BossSystem::spawnBoss(static_cast<BossType>(waveSystem.getCurrentWave() - 1));
        }
        BossBehaviorSystem::update(boss, playerPosition->value, asteroids, trapLasers, dt);

        // Обновление лазеров
        for (auto it = trapLasers.begin(); it != trapLasers.end();) {
            auto *laser = (*it)->getComponent<TrapLaser>();
            if (laser) {
                laser->update(dt);

                if (laser->timer >= laser->totalLifetime) {
                    it = trapLasers.erase(it);
                    continue;
                }

                // цвет рендера
                auto *render = (*it)->getComponent<Renderable>();
                if (render) {
                    render->shape.setFillColor(
                        laser->isActive ? TRAP_LASER_BASE_COLOR : TRAP_LASER_WARNING_COLOR
                    );
                }
            }
            ++it;
        }
    } else {
        // переход к следующей волне
        boss.reset();
        trapLasers.clear();
    }

    auto result = CollisionSystem::update(*player, bullets, asteroids, boss, trapLasers);

    if (result.isBossHit) {
        auto *bossHealth = boss ? boss->getComponent<Health>() : nullptr;
        if (bossHealth && bossHealth->value <= 0.f) {
            boss.reset();
            waveSystem.onBossDefeated();
        }
    }

    for (const auto [pos, vel, size]: result.asteroidsToSplit) {
        asteroidSpawnSystem.spawnChildAsteroids(
            asteroids,
            pos,
            vel,
            size
        );
    }

    waveSystem.update(result.destroyedAsteroidsCount, dt);

    EntityCleanupSystem::cleanupBullets(bullets);
    EntityCleanupSystem::cleanupAsteroids(asteroids);

    if (result.isPlayerDied) {
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

        if (auto *playerInvincibility = player->getComponent<Invincibility>()) {
            if (playerInvincibility->isActive()) {
                shouldDraw = playerInvincibility->shouldDraw();
            }
        }

        if (shouldDraw) {
            RenderSystem::render(window, {player.get()});
        }
    }

    RenderSystem::render(window, bullets);
    RenderSystem::render(window, asteroids);
    if (boss) {
        RenderSystem::render(window, {boss.get()});
    }
    RenderSystem::render(window, trapLasers);
}
