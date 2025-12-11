#pragma once
#include <memory>
#include <SFML/Graphics/RenderWindow.hpp>

class Entity;

class LaserSystem {
public:
    void update(
        std::unique_ptr<Entity> &player,
        std::vector<std::unique_ptr<Entity> > &laserBeams, // TODO: vector?
        const sf::RenderWindow &window,
        float dt
    );

    void applyDamage(
        std::vector<std::unique_ptr<Entity> > &laserBeams,
        std::vector<std::unique_ptr<Entity> > &asteroids,
        std::unique_ptr<Entity> &boss,
        float dt
    );
};
