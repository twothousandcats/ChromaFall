#include <SFML/Graphics.hpp>
#include "managers/GameStateManager.hpp"
#include "config/SetupConfig.hpp"
#include "config/UIConfig.hpp"

int main() {
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    sf::RenderWindow window(
        sf::VideoMode(WINDOW_SIZE),
        TITLE_TEXT,
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
