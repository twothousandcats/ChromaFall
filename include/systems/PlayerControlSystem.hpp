#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

class Entity;

class PlayerControlSystem {
public:
    void update(
        Entity &player,
        const sf::RenderWindow &window
    );
};
