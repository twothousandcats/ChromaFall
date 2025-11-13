#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "../include/entities/Entity.hpp"
#include "../include/components/Position.hpp"
#include "../include/components/Renderable.hpp"
#include "../include/systems/RenderSystem.hpp"
#include "systems/PlayerControlSystem.hpp"

constexpr unsigned WINDOW_WIDTH = 800.f;
constexpr unsigned WINDOW_HEIGHT = 600.f;
constexpr sf::Vector2u WINDOW_SIZE = {
    WINDOW_WIDTH,
    WINDOW_HEIGHT
};
constexpr sf::Vector2f WINDOW_CENTER = {
    WINDOW_WIDTH / 2.f,
    WINDOW_HEIGHT / 2.f
};
constexpr int FRAMES_PER_SECOND = 60;

int main() {
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    sf::RenderWindow window(
        sf::VideoMode(WINDOW_SIZE),
        "Chromafall",
        sf::Style::Default,
        sf::State::Windowed,
        settings
    );
    window.setFramerateLimit(FRAMES_PER_SECOND);

    // создание MC
    auto player = std::make_unique<Entity>();
    player->addComponent(std::make_unique<Position>(WINDOW_CENTER.x, WINDOW_CENTER.y));
    player->addComponent(std::make_unique<Renderable>());

    std::vector<std::unique_ptr<Entity>> entities;
    entities.push_back(std::move(player));

    RenderSystem renderSystem;
    PlayerControlSystem playerControlSystem;

    while (window.isOpen()) {
        renderSystem.pollEvents(window);

        playerControlSystem.update(*entities[0], window);

        renderSystem.update();
        renderSystem.render(window, entities);
    }
}
