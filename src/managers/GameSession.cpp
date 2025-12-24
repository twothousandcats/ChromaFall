#include "iostream"

#include "../../include/managers/GameSession.hpp"

#include "components/Experience.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/EntityCleanupSystem.hpp"
#include "systems/PlayerControlSystem.hpp"
#include "systems/RenderSystem.hpp"

#include "components/Health.hpp"
#include "components/Invincibility.hpp"
#include "components/Level.hpp"
#include "components/Position.hpp"
#include "components/Shape.hpp"
#include "components/Textured.hpp"
#include "components/TrapLaser.hpp"
#include "components/Upgrades.hpp"
#include "config/LevelConfig.hpp"

#include "config/PlayerConfig.hpp"
#include "config/UIConfig.hpp"
#include "managers/AudioManager.hpp"
#include "systems/BossBehaviorSystem.hpp"
#include "systems/BossSystem.hpp"
#include "systems/InvincibilitySystem.hpp"
#include "systems/PowerUpDropSystem.hpp"

// TODO: МАССОВОЕ ВНЕДРЕНИЕ ТЕКУСТУР
float GameSession::getPlayerHp() const {
    if (const auto *playerHp = player->getComponent<Health>()) {
        return playerHp->value;
    }

    return 0.f;
}

void GameSession::setOverlayState(OverlayState state) {
    if (state == OverlayState::PAUSE && overlayState == OverlayState::POWER_UP_SELECTION) {
        wasInPowerUp = true;
    } else if (state == OverlayState::NONE && wasInPowerUp) {
        overlayState = OverlayState::POWER_UP_SELECTION;
        wasInPowerUp = false;
        return;
    }
    overlayState = state;
}

void GameSession::selectNextPowerUp() {
    if (overlayState == OverlayState::POWER_UP_SELECTION && !powerUpOptions.empty()) {
        selectedPowerUpIndex = (selectedPowerUpIndex + 1) % powerUpOptions.size();
    }
}

void GameSession::selectPrevPowerUp() {
    if (overlayState == OverlayState::POWER_UP_SELECTION && !powerUpOptions.empty()) {
        selectedPowerUpIndex = (selectedPowerUpIndex + powerUpOptions.size() - 1) % powerUpOptions.size();
    }
}

void GameSession::confirmPowerUpSelection() {
    if (overlayState == OverlayState::POWER_UP_SELECTION && !powerUpOptions.empty()) {
        powerUpSystem.apply(player.get(), powerUpOptions[selectedPowerUpIndex]);
        overlayState = OverlayState::NONE;
    }
}

void GameSession::updateLaserWeapon(Position *playerPos, Upgrades *upgrades) {
    bool isShooting = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    float muzzleX = playerPos->value.x - LASER_WIDTH / 2.f;
    float muzzleY = playerPos->value.y - PLAYER_SIDE / 2.f;
    sf::Vector2f muzzlePos(muzzleX, muzzleY);
    float laserDamage = LASER_BASE_DMG * upgrades->damageMultiplier;

    laserSystem.update(
        playerLaser,
        window,
        muzzlePos,
        isShooting,
        true,
        laserDamage,
        std::max(0.f, playerPos->value.y)
    );
}

void GameSession::updateBulletWeapon(
    Position *playerPos,
    Upgrades *upgrades
) {
    int bulletCount = DEFAULT_BULLETS_COUNT + upgrades->extraBulletCount;
    float bulletDamage = BULLET_BASE_DMG * upgrades->damageMultiplier;
    float spreadAngle = DEFAULT_BULLETS_SPREAD_FACTOR;
    float shootingCooldown = SHOOTING_COOLDOWN * upgrades->shootingCooldown;

    if (upgrades->weapon == WeaponType::SHOTGUN) {
        bulletCount = SHOTGUN_BULLETS_COUNT + upgrades->extraBulletCount;
        spreadAngle = SHOTGUN_BULLETS_SPREAD_FACTOR;
    }

    auto *playerShape = player->getComponent<Shape>();
    if (!playerShape) return;

    sf::Vector2f muzzlePos = {
        playerPos->value.x,
        playerPos->value.y
    };

    shootSystem.update(
        bullets,
        muzzlePos,
        bulletCount,
        shootingCooldown,
        bulletDamage,
        spreadAngle,
        upgrades->weapon
    );
}

void GameSession::updateBackground(float dt) {
    currentScrollOffset += WINDOW_SCROLL_SPEED * dt;
    currentScrollOffset = std::fmod(currentScrollOffset, actualBgHeight);
}

void GameSession::updatePlayerAndWeapon(float dt) {
    InvincibilitySystem::update(player.get(), dt);
    PlayerControlSystem::update(*player, window);

    auto *playerPosition = player->getComponent<Position>();
    if (!playerPosition) return;

    auto *upgrades = player->getComponent<Upgrades>();
    if (!upgrades) return;

    if (upgrades->weapon != WeaponType::LASER) {
        playerLaser.reset();
    }

    if (upgrades->weapon == WeaponType::LASER) {
        updateLaserWeapon(playerPosition, upgrades);
    } else {
        updateBulletWeapon(playerPosition, upgrades);
    }
}

void GameSession::updateEntities(float dt) {
    movementSystem.update(bullets, dt);
    animationSystem.update(bullets, dt);
    movementSystem.update(asteroids, dt);
    animationSystem.update(asteroids, dt);
    movementSystem.update(powerUpPickups, dt);
    animationSystem.update(powerUpPickups, dt);
    if (boss) {
        movementSystem.update(*boss, dt);
        animationSystem.update(*boss, dt);
    }
}

void GameSession::updateAsteroidSpawning() {
    if (waveSystem.shouldSpawnAsteroids()) {
        asteroidSpawnSystem.update(
            asteroids,
            waveSystem.getCurrentWave()
        );
    }
}

void GameSession::updateBossAndTraps(float dt) {
    auto *playerPosition = player ? player->getComponent<Position>() : nullptr;
    if (!playerPosition) return;

    if (waveSystem.isBossPhase()) {
        if (!boss) {
            boss = bossSystem.spawnBoss(static_cast<BossType>(waveSystem.getCurrentWave() - 1));
        }
        bossBehaviorSystem.update(boss, playerPosition->value, asteroids, trapLasers, dt);

        for (auto it = trapLasers.begin(); it != trapLasers.end();) {
            auto *laser = (*it)->getComponent<TrapLaser>();
            if (laser) {
                laser->update(dt);
                if (laser->timer >= laser->totalLifetime) {
                    it = trapLasers.erase(it);
                    continue;
                }
                auto *render = (*it)->getComponent<Shape>();
                if (render) {
                    render->shape.setFillColor(
                        laser->isActive ? TRAP_LASER_BASE_COLOR : TRAP_LASER_WARNING_COLOR
                    );
                }
            }
            ++it;
        }
    } else {
        boss.reset();
        trapLasers.clear();
    }
}

void GameSession::handleLevelUpAndPowerUps() {
    if (experienceSystem.processLevelUps(player.get())) {
        AudioManager::getInstance().playSound(AUDIO_SFX_LVLUP_NAME);
        overlayState = OverlayState::POWER_UP_SELECTION;
        if (auto *upgrades = player->getComponent<Upgrades>()) {
            powerUpOptions = powerUpSystem.generateOptions(3, upgrades->weapon);
        }
        selectedPowerUpIndex = 0;
    }
}

void GameSession::processCollisions(float dt) {
    Entity *laserPtr = playerLaser ? playerLaser.get() : nullptr;
    auto result = CollisionSystem::update(
        *player,
        laserPtr,
        bullets,
        asteroids,
        boss,
        trapLasers,
        powerUpPickups
    );
    for (PowerUpType type: result.collectedPowerUps) {
        powerUpSystem.apply(player.get(), type);
    }
    // todo: резать длину лазера? но определение размеров происходит раньше

    experienceSystem.onAsteroidsDestroyed(result.destroyedAsteroidsCount);
    // Дроп усилений
    if (auto *upgrades = player->getComponent<Upgrades>()) {
        for (const sf::Vector2f &pos: result.destroyedAsteroidPositions) {
            powerUpDropSystem.tryDropPowerUp(
                powerUpPickups,
                pos,
                upgrades->weapon,
                droppedPowerUpRng
            );
        }
    }

    if (result.isBossHit) {
        auto *bossHealth = boss ? boss->getComponent<Health>() : nullptr;
        if (bossHealth && bossHealth->value <= 0.f) {
            boss.reset();
            experienceSystem.onBossDefeated(
                static_cast<BossType>(waveSystem.getCurrentWave() - 1)
            );

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

    handleLevelUpAndPowerUps();

    EntityCleanupSystem::cleanupBullets(bullets);
    EntityCleanupSystem::cleanupAsteroids(asteroids);
    waveSystem.update(result.destroyedAsteroidsCount, dt);

    if (result.isPlayerDied) {
        gameOver = true;
    }
}

int GameSession::getPlayerLevel() const {
    if (const auto *lvl = player->getComponent<Level>()) {
        return lvl->value;
    }

    return LEVEL_BASE;
}

int GameSession::getPlayerCurrentExp() const {
    if (const auto *exp = player->getComponent<Experience>()) {
        return exp->value;
    }

    return LEVEL_START_EXP;
}

int GameSession::getPlayerExpForNextLevel() const {
    if (const auto *exp = player->getComponent<Experience>()) {
        return static_cast<int>(exp->neededValue);
    }

    return LEVEL_BASE_NEEDED_COUNT_EXP;
}

void GameSession::createPlayer() {
    if (!playerTexture.loadFromFile(PLAYER_TEXTURE)) {
        exit(1);
    }

    player = std::make_unique<Entity>();
    player->addComponent(std::make_unique<Position>());
    player->addComponent(std::make_unique<Textured>(playerTexture));
    player->addComponent(std::make_unique<Shape>(PLAYER_SIDE, PLAYER_SIDE, PLAYER_COLOR));
    player->addComponent(std::make_unique<Health>(PLAYER_BASE_HP));
    player->addComponent(std::make_unique<Invincibility>());
    player->addComponent(std::make_unique<Experience>(LEVEL_START_EXP));
    player->addComponent(std::make_unique<Level>(LEVEL_BASE));
    player->addComponent(std::make_unique<Upgrades>());
}

GameSession::GameSession(sf::RenderWindow &window) : window(window) {
    init();

    if (bgTexture.loadFromFile(BASE_BACKGROUND)) {
        bgSprite.emplace(bgTexture);

        float scaleX = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(bgTexture.getSize().x);
        float scaleY = static_cast<float>(WINDOW_HEIGHT) / static_cast<float>(bgTexture.getSize().y);
        bgSprite->setScale({scaleX, scaleY});

        actualBgHeight = bgSprite->getLocalBounds().size.y;
    } else {
        exit(1);
    }
}

void GameSession::init() {
    createPlayer();

    bullets.clear();
    asteroids.clear();
    gameOver = false;

    droppedPowerUpRng.seed(std::random_device{}());
}

void GameSession::update(
    const float dt
) {
    // конец игры/lvlup/pause
    if (gameOver || overlayState != OverlayState::NONE) {
        return;
    }

    updateBackground(dt);
    updatePlayerAndWeapon(dt);
    updateEntities(dt);
    updateAsteroidSpawning();
    updateBossAndTraps(dt);
    processCollisions(dt);
}

void GameSession::render(
    sf::RenderWindow &window
) {
    if (bgSprite.has_value()) {
        bgSprite->setPosition({0, -currentScrollOffset});
        window.draw(*bgSprite);

        // Второй спрайт — смещён на actualBgHeight вверх
        sf::Sprite second = *bgSprite;
        second.setPosition({0, -currentScrollOffset - actualBgHeight});
        window.draw(second);
    }

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
    if (playerLaser) {
        RenderSystem::render(window, {playerLaser.get()});
    }
    RenderSystem::render(window, bullets);
    RenderSystem::render(window, asteroids);
    if (boss) {
        RenderSystem::render(window, {boss.get()});
    }

    RenderSystem::render(window, powerUpPickups);
    RenderSystem::render(window, trapLasers);
}
