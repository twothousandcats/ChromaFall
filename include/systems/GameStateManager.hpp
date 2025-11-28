#include "core/GameState.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <random>
#include <vector>

#include "MovementSystem.hpp"
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

    void spawnAsteroid();

    void checkCollisions();

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
    sf::Color titleColor = {147, 30, 119, 255};
    int titleFontSize = 40;
    std::string gameTitle = "ChromaFall";

    std::optional<sf::Text> startText;
    std::string startTextValue = "Start";
    sf::RectangleShape startButton;

    std::optional<sf::Text> exitText;
    std::string exitTextValue = "Exit";
    sf::RectangleShape exitButton;

    sf::Vector2f buttonSize = {200.f, 60.f};
    sf::Color buttonColor = {0, 204, 109, 255};
    sf::Color buttonTextColor = {255, 255, 255, 255};
    int buttonFontSize = 32;
    float buttonGap = 40.f + buttonSize.y;
    std::string defaultFont = "assets/fonts/Orbitron-Regular.ttf";
    std::string boldFont = "assets/fonts/Orbitron-Bold.ttf";

    std::unique_ptr<GameSession> gameSession;
    sf::Clock gameClock;
    sf::Clock victoryClock;
};
