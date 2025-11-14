#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

class Entity;

/**
 * Система управления MC
 */
class PlayerControlSystem {
public:
    static void update(
        Entity &player,
        const sf::RenderWindow &window
    );
};
