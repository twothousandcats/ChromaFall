#include "core/GameState.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class Entity;

class GameStateManager {
public:
    GameStateManager(sf::RenderWindow &window);

    ~GameStateManager(); // деструктор

    void handleEvents();

    void update();

    void render();

    GameState getCurrentState() const { return currentState; }

private:
    void switchToMainMenu();

    void switchToGameplay();

    sf::RenderWindow &window;
    GameState currentState = GameState::MainMenu;

    // Меню
    sf::Vector2f menuButtonCenter;
    float menuButtonRadius = 20.f;

    // Игра
    std::vector<std::unique_ptr<Entity> > entities;
    class RenderSystem *renderSystem = nullptr;
    class PlayerControlSystem *playerControlSystem = nullptr;
};
