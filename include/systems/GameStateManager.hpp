#pragma once

#include "core/GameState.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <random>
#include <vector>

#include "ShootSystem.hpp"

class Entity;
class GameSession;

/**
 * Система управления состояниями программы
 */
class GameStateManager {
public:
    explicit GameStateManager(sf::RenderWindow &window);

    ~GameStateManager(); // деструктор

    void handleEvents();

    void update();

    void render();

    [[nodiscard]] GameState getCurrentState() const { return currentState; }

private:
    void switchToMainMenu();

    void switchToGameplay();

    static sf::Text createText(
        const std::string &textContent,
        const sf::Vector2f &positionCenter,
        const sf::Font &font,
        unsigned int charSize,
        const sf::Color &textColor,
        const sf::Text::Style &textStyle = sf::Text::Bold
    );

    static std::pair<sf::RectangleShape, sf::Text> createButton(
        const std::string &text,
        const sf::Vector2f &positionCenter,
        const sf::Font &font,
        unsigned int charSize = 32,
        const sf::Color &bgColor = sf::Color({0, 204, 109, 255}),
        const sf::Color &textColor = sf::Color({255, 255, 255, 255})
    );

    sf::RenderWindow &window;
    GameState currentState = GameState::MainMenu;

    // UI Меню
    sf::Font font;
    std::optional<sf::Text> titleText;

    std::optional<sf::Text> startText;
    sf::RectangleShape startButton;

    std::optional<sf::Text> exitText;
    sf::RectangleShape exitButton;

    std::unique_ptr<GameSession> gameSession;
    sf::Clock gameClock;
    sf::Clock infoClock;
};
