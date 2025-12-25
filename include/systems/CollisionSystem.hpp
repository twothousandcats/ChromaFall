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

    Result update(
        Entity &player,
        Entity *playerLaser,
        std::vector<std::unique_ptr<Entity> > &bullets,
        std::vector<std::unique_ptr<Entity> > &asteroids,
        std::unique_ptr<Entity> &boss,
        const std::vector<std::unique_ptr<Entity> > &trapLasers,
        std::vector<std::unique_ptr<Entity> > &powerUpPickups
    ) const;

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

    static void processBulletAsteroidCollisions(
        std::vector<std::unique_ptr<Entity> > &bullets,
        std::vector<std::unique_ptr<Entity> > &asteroids,
        Result &result
    );

    static void processAsteroidPlayerCollisions(
        Entity &player,
        const std::vector<std::unique_ptr<Entity> > &asteroids,
        Result &result
    );

    static void processBulletBossCollisions(
        std::vector<std::unique_ptr<Entity> > &bullets,
        const std::unique_ptr<Entity> &boss,
        Result &result
    );

    static void processTrapLaserPlayerCollisions(
        Entity &player,
        const std::vector<std::unique_ptr<Entity> > &trapLasers,
        Result &result
    );

    static void processPlayerLaserCollisions(
        Entity &playerLaser,
        std::vector<std::unique_ptr<Entity> > &asteroids,
        const std::unique_ptr<Entity> &boss,
        Result &result
    );

    static void processPowerUpPickups(
        Entity &player,
        std::vector<std::unique_ptr<Entity> > &powerUpPickups,
        Result &result
    );
};
