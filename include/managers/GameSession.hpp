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
#include "config/WaveConfig.hpp"
#include "data/OverlayState.hpp"
#include "entities/Entity.hpp"

enum class PowerUpType;
enum class OverlayState;

class GameSession {
public:
    explicit GameSession(sf::RenderWindow &window);

    void update(float dt);

    void render(sf::RenderWindow &window);

    void init();

    void setOverlayState(OverlayState state);

    [[nodiscard]] float getPlayerHp() const;

    [[nodiscard]] bool isGameOver() const { return gameOver; }

    [[nodiscard]] int getCurrentWave() const { return waveSystem.getCurrentWave(); }

    [[nodiscard]] int getTotalWaves() const {
        return waveSystem.getTotalWaves();
    }

    [[nodiscard]] bool isVictory() const {
        return waveSystem.isVictory();
    }

    [[nodiscard]] OverlayState getOverlayState() const { return overlayState; }

    // UI
    [[nodiscard]] const std::vector<PowerUpType>& getPowerUpOptions() const { return powerUpOptions; }
    [[nodiscard]] int getSelectedPowerUpIndex() const { return selectedPowerUpIndex; }
    void selectNextPowerUp();
    void selectPrevPowerUp();
    void confirmPowerUpSelection();

private:
    // сущности
    std::unique_ptr<Entity> player;
    // снаряды
    std::unique_ptr<Entity> playerLaser; // ← переместить из временной переменной сюда
    std::vector<std::unique_ptr<Entity> > bullets;
    // противники
    std::vector<std::unique_ptr<Entity> > asteroids;
    std::unique_ptr<Entity> boss;
    // ловушки
    std::vector<std::unique_ptr<Entity> > trapLasers;

    // системы
    WaveSystem waveSystem{TOTAL_WAVES, KILLS_TO_WAVE_UP};
    MovementSystem movementSystem;
    ShootSystem shootSystem;
    AsteroidSpawnSystem asteroidSpawnSystem;
    ExperienceSystem experienceSystem;
    LaserSystem laserSystem;

    // общее
    sf::RenderWindow &window;
    sf::Clock gameClock;

    bool gameOver = false; // state

    // powerup
    PowerUpSystem powerUpSystem;
    bool awaitingPowerUp = false;
    std::vector<PowerUpType> currentPowerUpOptions;

    // состяние
    OverlayState overlayState = OverlayState::NONE;
    bool wasInPowerUp = false; // чтобы вернуться после паузы

    // PowerUpScreen
    std::vector<PowerUpType> powerUpOptions;
    int selectedPowerUpIndex = 0;
};
