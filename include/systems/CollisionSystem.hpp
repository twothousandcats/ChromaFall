#pragma once
#include <memory>
#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include "data/AsteroidSize.hpp"

class Entity;

class CollisionSystem {
public:
    struct Result {
        bool isPlayerHit = false;
        bool isPlayerDied = false;
        int destroyedAsteroidsCount = 0;
        std::vector<std::tuple<sf::Vector2f, sf::Vector2f, AsteroidSize> > asteroidsToSplit;
        bool isBossHit = false;
    };

    static Result update(
        Entity &player,
        std::vector<std::unique_ptr<Entity> > &bullets,
        std::vector<std::unique_ptr<Entity> > &asteroids,
        std::unique_ptr<Entity> &boss,
        const std::vector<std::unique_ptr<Entity> > &trapLasers
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
