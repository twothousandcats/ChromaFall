#pragma once
#include <random>
#include <SFML/Graphics/RenderWindow.hpp>

#include "AsteroidSpawnSystem.hpp"
#include "ExperienceSystem.hpp"
#include "MovementSystem.hpp"
#include "ShootSystem.hpp"
#include "WaveSystem.hpp"
#include "config/WaveConfig.hpp"
#include "entities/Entity.hpp"
#include "systems/BossSystem.hpp"
#include "systems/BossBehaviorSystem.hpp"

class GameSession {
public:
    explicit GameSession(sf::RenderWindow &window);

    void update(float dt);

    void render(sf::RenderWindow &window);

    void init();

    [[nodiscard]] float getPlayerHp() const;

    [[nodiscard]] bool isGameOver() const { return gameOver; }

    [[nodiscard]] int getCurrentWave() const { return waveSystem.getCurrentWave(); }

    [[nodiscard]] int getTotalWaves() const {
        return waveSystem.getTotalWaves();
    }

    [[nodiscard]] bool isVictory() const {
        return waveSystem.isVictory();
    }

private:
    // сущности
    std::unique_ptr<Entity> player;
    std::vector<std::unique_ptr<Entity> > bullets;
    std::vector<std::unique_ptr<Entity> > asteroids;
    std::unique_ptr<Entity> boss;
    std::vector<std::unique_ptr<Entity>> trapLasers;

    // системы
    WaveSystem waveSystem{TOTAL_WAVES, KILLS_TO_WAVE_UP};
    MovementSystem movementSystem;
    ShootSystem shootSystem;
    AsteroidSpawnSystem asteroidSpawnSystem;
    ExperienceSystem experienceSystem;

    // общее
    sf::RenderWindow &window;
    sf::Clock gameClock;

    bool gameOver = false; // state
};