#pragma once
#include <random>
#include <SFML/Graphics/RenderWindow.hpp>

#include "../systems/AsteroidSpawnSystem.hpp"
#include "../systems/ExperienceSystem.hpp"
#include "../systems/LaserSystem.hpp"
#include "../systems/MovementSystem.hpp"
#include "../systems/PowerUpSystem.hpp"
#include "../systems/ShootSystem.hpp"
#include "../systems/WaveSystem.hpp"
#include "components/Position.hpp"
#include "config/SetupConfig.hpp"
#include "config/WaveConfig.hpp"
#include "data/OverlayState.hpp"
#include "entities/Entity.hpp"
#include "systems/BossBehaviorSystem.hpp"
#include "systems/BossSystem.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/PowerUpDropSystem.hpp"
#include "systems/TextureAnimationSystem.hpp"

enum class PowerUpType;
enum class OverlayState;

class GameSessionManager {
public:
    explicit GameSessionManager(sf::RenderWindow &window);

    void update(float dt);

    void render(sf::RenderWindow &window);

    void setOverlayState(OverlayState state);

    [[nodiscard]] float getPlayerHp() const;

    [[nodiscard]] bool isGameOver() const { return gameOver; }

    // геттеры для UI волны
    [[nodiscard]] int getCurrentWave() const { return waveSystem.getCurrentWave(); }

    [[nodiscard]] int getTotalWaves() const {
        return waveSystem.getTotalWaves();
    }

    [[nodiscard]] bool isVictory() const {
        return waveSystem.isVictory();
    }

    [[nodiscard]] int getCurrentWaveKilledCount() const {
        return waveSystem.getCurrentWaveKills();
    }

    [[nodiscard]] int getCurrentWaveTotalCount() const {
        return waveSystem.getCurrentWaveRequiredKills();
    }

    [[nodiscard]] OverlayState getOverlayState() const { return overlayState; }

    // UI
    [[nodiscard]] const std::vector<PowerUpType> &getPowerUpOptions() const { return powerUpOptions; }
    [[nodiscard]] int getSelectedPowerUpIndex() const { return selectedPowerUpIndex; }

    void selectNextPowerUp();

    void selectPrevPowerUp();

    void confirmPowerUpSelection();

    // гетеры для UI
    int getPlayerLevel() const;

    int getPlayerCurrentExp() const;

    int getPlayerExpForNextLevel() const;
    std::string getPlayerWeaponName() const;
    std::string getLastAppliedPowerUpName() const;

private:
    void init();

    void createPlayer();

    void updateBackground(float dt);

    void updatePlayerAndWeapon(float dt);

    void updateLaserWeapon(Position *playerPos, Upgrades *upgrades);

    void updateBulletWeapon(Position *playerPos, Upgrades *upgrades);

    void updateEntities(float dt);

    void updateAsteroidSpawning();

    void updateBossAndTraps(float dt);

    void processCollisions(float dt);

    void handleLevelUpAndPowerUps();

    // сущности
    std::unique_ptr<Entity> player;
    sf::Texture playerTexture; // рендер MC
    // снаряды
    std::unique_ptr<Entity> playerLaser;
    std::vector<std::unique_ptr<Entity> > bullets;
    // противники
    std::vector<std::unique_ptr<Entity> > asteroids;
    std::unique_ptr<Entity> boss;
    // ловушки
    std::vector<std::unique_ptr<Entity> > trapLasers;
    // улучшения
    std::vector<std::unique_ptr<Entity> > powerUpPickups;

    // системы
    CollisionSystem collisionSystem;
    WaveSystem waveSystem{TOTAL_WAVES, KILLS_TO_WAVE_UP};
    BossSystem bossSystem;
    BossBehaviorSystem bossBehaviorSystem;
    MovementSystem movementSystem;
    ShootSystem shootSystem;
    AsteroidSpawnSystem asteroidSpawnSystem;
    ExperienceSystem experienceSystem;
    LaserSystem laserSystem;
    TextureAnimationSystem animationSystem;
    PowerUpSystem powerUpSystem;
    PowerUpDropSystem powerUpDropSystem;

    // общее
    sf::RenderWindow &window;
    sf::Clock gameClock;

    bool gameOver = false; // state

    // powerup
    bool awaitingPowerUp = false;
    std::vector<PowerUpType> currentPowerUpOptions;
    // для случайных улучшений
    std::mt19937 droppedPowerUpRng;

    // состяние
    OverlayState overlayState = OverlayState::NONE;
    bool wasInPowerUp = false; // чтобы вернуться после паузы

    // PowerUpScreen
    std::vector<PowerUpType> powerUpOptions;
    int selectedPowerUpIndex = 0;
    std::optional<PowerUpType> lastAppliedPowerUp;

    // background
    sf::Texture bgTexture;
    std::optional<sf::Sprite> bgSprite;
    float currentScrollOffset = 0.f;
    float actualBgHeight = 0.f;
};
