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
#include "systems/PowerUpDropSystem.hpp"
#include "systems/TextureAnimationSystem.hpp"

enum class PowerUpType;
enum class OverlayState;

class GameSession {
public:
    explicit GameSession(sf::RenderWindow &window);

    void update(float dt);

    void render(sf::RenderWindow &window);

    void init();

    void setOverlayState(OverlayState state);
    void createPlayer();

    [[nodiscard]] float getPlayerHp() const;

    [[nodiscard]] bool isGameOver() const { return gameOver; }

    // прокидываем UI данные волны
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

    // гетеры для UI exp
    int getPlayerLevel() const;
    int getPlayerCurrentExp() const;
    int getPlayerExpForNextLevel() const;

private:
    void updateBackground(float dt);
    void updatePlayerAndWeapon(float dt);
    void updateLaserWeapon(Position* playerPos, Upgrades* upgrades);
    void updateBulletWeapon(Position* playerPos, Upgrades* upgrades);
    void updateEntities(float dt);
    void updateAsteroidSpawning();
    void updateBossAndTraps(float dt);
    void processCollisions(float dt);
    void handleLevelUpAndPowerUps();

    // сущности
    std::unique_ptr<Entity> player;
    sf::Texture playerTexture; // рендер MC
    // снаряды
    std::unique_ptr<Entity> playerLaser; // ← переместить из временной переменной сюда
    std::vector<std::unique_ptr<Entity> > bullets;
    // противники
    std::vector<std::unique_ptr<Entity> > asteroids;
    std::unique_ptr<Entity> boss;
    // ловушки
    std::vector<std::unique_ptr<Entity> > trapLasers;
    // улучшения
    std::vector<std::unique_ptr<Entity>> powerUpPickups;

    // системы
    WaveSystem waveSystem{TOTAL_WAVES, KILLS_TO_WAVE_UP};
    BossSystem bossSystem;
    BossBehaviorSystem bossBehaviorSystem;
    MovementSystem movementSystem;
    ShootSystem shootSystem;
    AsteroidSpawnSystem asteroidSpawnSystem;
    ExperienceSystem experienceSystem;
    LaserSystem laserSystem;
    TextureAnimationSystem animationSystem;

    // общее
    sf::RenderWindow &window;
    sf::Clock gameClock;

    bool gameOver = false; // state

    // powerup
    PowerUpSystem powerUpSystem;
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

    // background
    sf::Texture bgTexture;
    std::optional<sf::Sprite> bgSprite;
    float currentScrollOffset = 0.f;
    float actualBgHeight = 0.f;
};
