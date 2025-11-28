#pragma once
#include <memory>
#include <vector>
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
    };

    static Result update(
        Entity &player,
        std::vector<std::unique_ptr<Entity> > &bullets,
        std::vector<std::unique_ptr<Entity> > &asteroids
    );

private:
    static bool isIntersects(
        const sf::Vector2f &aPos,
        const sf::Vector2f &bPos,
        const sf::Vector2f &aSize,
        const sf::Vector2f &bSize
    );
};
