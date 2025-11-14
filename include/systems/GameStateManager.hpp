#include "core/GameState.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class Entity;

/**
 * Система управления состояниями программы
 */
class GameStateManager {
public:
    explicit GameStateManager(sf::RenderWindow &window);
    ~GameStateManager(); // деструктор

    void handleEvents();

    void update() const;

    void render() const;

    [[nodiscard]] GameState getCurrentState() const { return currentState; }

private:
    void switchToMainMenu();

    void switchToGameplay();

    static std::pair<sf::RectangleShape, sf::Text> createButton(
        const std::string& text,
        const sf::Vector2f& positionCenter,
        const sf::Font& font,
        unsigned int charSize = 32,
        const sf::Color& bgColor = sf::Color(50, 50, 50),
        const sf::Color& textColor = sf::Color::White
    );

    sf::RenderWindow &window;
    GameState currentState = GameState::MainMenu;

    // Меню
    sf::Font font;
    std::optional<sf::Text> titleText;
    sf::Color titleColor = {147, 30, 119, 100};
    int titleFontSize = 40;

    std::optional<sf::Text> startText;
    std::string startTextValue = "Start";
    sf::RectangleShape startButton;

    std::optional<sf::Text> exitText;
    std::string exitTextValue = "Exit";
    sf::RectangleShape exitButton;

    sf::Vector2f buttonSize = {200.f, 60.f};
    sf::Color buttonColor = {0, 204, 109, 80};
    sf::Color buttonTextColor = {255, 255, 255, 100};
    int buttonFontSize = 32;

    // Игра
    std::vector<std::unique_ptr<Entity> > entities;
    class RenderSystem *renderSystem = nullptr;
    class PlayerControlSystem *playerControlSystem = nullptr;
};
