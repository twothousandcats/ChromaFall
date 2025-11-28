#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>

class Entity;

class RenderSystem {
public:
    static void pollEvents(
        sf::RenderWindow &window
    ) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
    }

    static void update(
    ) {
    }

    static void render(
        sf::RenderWindow &window,
        const std::vector<Entity *> &entities
    );

    // перегрузочный метод
    static void render(
        sf::RenderWindow &window,
        const std::vector<std::unique_ptr<Entity> > &entities
    );
};
