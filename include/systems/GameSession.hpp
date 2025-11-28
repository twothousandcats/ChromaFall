#pragma once
#include <random>
#include <SFML/Graphics/RenderWindow.hpp>

#include "MovementSystem.hpp"
#include "ShootSystem.hpp"
#include "WaveSystem.hpp"
#include "config/WaveConfig.hpp"
#include "data/AsteroidSize.hpp"
#include "systems/CollisionSystem.hpp"
#include "entities/Entity.hpp"

class GameSession {
public:
    explicit GameSession(sf::RenderWindow &window);

    void update(float dt);

    void render(sf::RenderWindow &window);

    [[nodiscard]] float getPlayerHp() const;

    void reset();

    [[nodiscard]] bool isGameOver() const { return gameOver; }

    [[nodiscard]] int getCurrentWave() const { return waveSystem.getCurrentWave(); }

    [[nodiscard]] int getTotalWaves() const {
        return waveSystem.getTotalWaves();
    }

    [[nodiscard]] bool isVictory() const {
        return waveSystem.isVictory();
    }

private:
    void setupEntities(
        sf::RenderWindow &window,
        const std::vector<std::unique_ptr<Entity> > &entities
    );

    void cleanEntities();

    void spawnAsteroid();

    void spawnChildAsteroid(
        const sf::Vector2f &parentPos,
        const sf::Vector2f &parentVel,
        AsteroidSize parentSize
    );

    sf::RenderWindow &window;
    // сущности
    std::unique_ptr<Entity> player;
    std::vector<std::unique_ptr<Entity> > bullets;
    std::vector<std::unique_ptr<Entity> > asteroids;

    WaveSystem waveSystem{TOTAL_WAVES, KILLS_TO_WAVE_UP};
    MovementSystem movementSystem;
    ShootSystem shootSystem;
    sf::Clock asteroidSpawnClock;
    sf::Clock gameClock;

    // генератор
    std::mt19937 randomEngine;
    std::uniform_int_distribution<int> asteroidTypeDist;
    std::uniform_real_distribution<float> asteroidIntervalDist;
    float currentSpawnInterval = 1.f;

    bool gameOver = false;
    int bulletsCount = 1;
    float spreadAngle = 0.3f;
};
