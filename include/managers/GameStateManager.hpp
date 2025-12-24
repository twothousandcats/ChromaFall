#pragma once

#include "core/GameState.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <random>
#include <vector>

#include "../systems/ShootSystem.hpp"
#include "config/SetupConfig.hpp"
#include "data/TextOrigin.hpp"

enum class TextOrigin;
class Entity;
class GameSession;

enum class PauseOption { RESUME, MAIN_MENU };

/**
 * Система управления состояниями программы
 */
class GameStateManager {
public:
    explicit GameStateManager(sf::RenderWindow &window);

    ~GameStateManager();

    void handleEvents();

    void update();

    void render();

    [[nodiscard]] GameState getCurrentState() const { return currentState; }

private:
    void switchToMainMenu();

    void switchToGameplay();

    void renderPowerUpOverlay(
        sf::RenderWindow &window
    );

    void renderPauseOverlay(
        sf::RenderWindow &window
    );

    static sf::Text createText(
        const std::string &textContent,
        const sf::Vector2f &positionCenter,
        const sf::Font &font,
        unsigned int charSize,
        const sf::Color &textColor,
        const sf::Text::Style &textStyle = sf::Text::Bold,
        const TextOrigin origin = TextOrigin::CENTER
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

    // UI pause
    PauseOption selectedPauseOption = PauseOption::RESUME;

    std::unique_ptr<GameSession> gameSession;
    sf::Clock gameClock;
    sf::Clock infoClock;
    sf::Clock blinkClock;

    // hp
    sf::Texture hpTexture;
    std::optional<sf::Sprite> hpSprite;

    // music
    bool isThemeMusicPlaying = false;
    bool isGameMusicPlaying = false;

    // sounds
};
