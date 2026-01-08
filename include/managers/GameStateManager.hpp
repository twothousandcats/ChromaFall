#pragma once

#include "core/GameState.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <random>
#include <vector>

#include "../systems/ShootSystem.hpp"
#include "config/SetupConfig.hpp"
#include "config/UIConfig.hpp"
#include "data/TextOrigin.hpp"

enum class TextOrigin;
class Entity;
class GameSessionManager;

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
        const sf::Color &bgColor = BUTTON_BG_COLOR,
        const sf::Color &textColor = BUTTON_TEXT_COLOR
    );

    void handleMainMenuEvents(const sf::Event &event);

    void handlePowerUpSelectionInput(sf::Keyboard::Key key);

    void handlePauseMenuInput(sf::Keyboard::Key key);

    void handleGameplayEvents(const sf::Event &event);

    void updateGameplay(float deltaTime);

    void updateResultScreen();

    void renderMainMenu();

    void renderGameplay();

    void renderGameplayHUD();

    void renderGameplayOverlays();

    void renderVictory();

    void renderGameOver();

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

    std::unique_ptr<GameSessionManager> gameSessionManager;
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
