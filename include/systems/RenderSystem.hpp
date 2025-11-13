#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>
#include <memory>

class Entity;

class RenderSystem {
public:
    void pollEvents(
        sf::RenderWindow &window
    ) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
    }

    void update(
    ) {
    }

    void render(
        sf::RenderWindow &window,
        std::vector<std::unique_ptr<Entity> > &entities
    );
};
