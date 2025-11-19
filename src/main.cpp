#include <SFML/Graphics.hpp>
#include "../include/systems/GameStateManager.hpp"

constexpr unsigned WINDOW_WIDTH = 800;
constexpr unsigned WINDOW_HEIGHT = 600;
constexpr sf::Vector2u WINDOW_SIZE = {WINDOW_WIDTH, WINDOW_HEIGHT};
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

    GameStateManager stateManager(window);

    while (window.isOpen()) {
        stateManager.handleEvents();
        stateManager.update();
        stateManager.render();
    }

    return 0;
}
