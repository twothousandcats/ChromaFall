#pragma once
#include <random>
#include <SFML/Graphics/RenderWindow.hpp>

#include "MovementSystem.hpp"
#include "ShootSystem.hpp"
#include "entities/Entity.hpp"

class GameSession {
public:
    explicit GameSession(sf::RenderWindow &window);

    void update(float dt);

    void render(sf::RenderWindow &window);

    void reset();

    bool isGameOver() const { return gameOver; }

    [[nodiscard]] float getPlayerHp() const;

private:
    void setupEntities(
        sf::RenderWindow &window,
        const std::vector<std::unique_ptr<Entity> > &entities
    );

    void cleanEntities();

    void spawnAsteroid();

    void checkCollision();

    bool isIntersects(
        const sf::Vector2f &aPos,
        const sf::Vector2f &bPos,
        const sf::Vector2f &aSize,
        const sf::Vector2f &bSize
    );

    sf::RenderWindow &window;
    // сущности
    std::unique_ptr<Entity> player;
    std::vector<std::unique_ptr<Entity> > bullets;
    std::vector<std::unique_ptr<Entity> > asteroids;

    MovementSystem movementSystem;
    ShootSystem shootSystem;
    sf::Clock asteroidSpawnClock;

    // генератор
    std::mt19937 randomEngine;
    std::uniform_int_distribution<int> asteroidTypeDist;
    std::uniform_real_distribution<float> asteroidIntervalDist;
    float currentSpawnInterval = 1.f;

    bool gameOver = false;
    int bulletsCount = 1;
    float spreadAngle = 0.3f;
};
