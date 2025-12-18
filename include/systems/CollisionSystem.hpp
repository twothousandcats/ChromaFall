#pragma once
#include <memory>
#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include "data/AsteroidSize.hpp"
#include "data/PowerUpType.hpp"

class Entity;

class CollisionSystem {
public:
    struct Result {
        bool isPlayerHit = false;
        bool isPlayerDied = false;
        int destroyedAsteroidsCount = 0;
        std::vector<std::tuple<sf::Vector2f, sf::Vector2f, AsteroidSize> > asteroidsToSplit;
        std::vector<sf::Vector2f> destroyedAsteroidPositions;
        std::vector<PowerUpType> collectedPowerUps;
        bool isBossHit = false;
        int expEarned = 0;
    };

    static Result update(
        Entity &player,
        Entity *playerLaser,
        std::vector<std::unique_ptr<Entity> > &bullets,
        std::vector<std::unique_ptr<Entity> > &asteroids,
        std::unique_ptr<Entity> &boss,
        const std::vector<std::unique_ptr<Entity> > &trapLasers,
    std::vector<std::unique_ptr<Entity>>& powerUpPickups
    );

private:
    static bool isIntersects(
        const sf::Vector2f &aPos,
        const sf::Vector2f &bPos,
        const sf::Vector2f &aSize,
        const sf::Vector2f &bSize
    );

    static float distanceFromPointToSegment(
        const sf::Vector2f &point,
        const sf::Vector2f &segA,
        const sf::Vector2f &segB
    );
};
