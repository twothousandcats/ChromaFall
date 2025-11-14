#include "../include/systems/GameStateManager.hpp"
#include "../include/entities/Entity.hpp"
#include "../include/components/Position.hpp"
#include "../include/components/Renderable.hpp"
#include "../include/systems/RenderSystem.hpp"
#include "../include/systems/PlayerControlSystem.hpp"

#include <SFML/Graphics.hpp>
#include <optional>

constexpr unsigned int WINDOW_WIDTH = 800;
constexpr unsigned int WINDOW_HEIGHT = 600;

GameStateManager::GameStateManager(sf::RenderWindow &window)
    : window(window) {
    menuButtonCenter = {WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f};
}

GameStateManager::~GameStateManager() = default;

void GameStateManager::switchToMainMenu() {
    currentState = GameState::MainMenu;
    entities.clear();
    delete renderSystem;
    delete playerControlSystem;
    renderSystem = nullptr;
    playerControlSystem = nullptr;
}

void GameStateManager::switchToGameplay() {
    currentState = GameState::Gameplay;

    // создаем MC
    auto player = std::make_unique<Entity>();
    player->addComponent(std::make_unique<Position>());
    player->addComponent(std::make_unique<Renderable>());
    entities.push_back(std::move(player));

    renderSystem = new RenderSystem();
    playerControlSystem = new PlayerControlSystem();
}

void GameStateManager::handleEvents() {
    while (const auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        } else if (event->is<sf::Event::KeyPressed>()) {
            const auto &keyEvent = event->getIf<sf::Event::KeyPressed>();
            if (keyEvent->code == sf::Keyboard::Key::Space) {
                if (currentState == GameState::MainMenu) {
                    switchToGameplay();
                }
            }
        } else if (currentState == GameState::MainMenu) {
            if (event->is<sf::Event::MouseButtonPressed>()) {
                const auto &mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    // Получаем позицию мыши в координатах окна
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePosF = {static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)};

                    // Проверяем расстояние до центра кнопки
                    sf::Vector2f delta = mousePosF - menuButtonCenter;
                    float distanceSquared = delta.x * delta.x + delta.y * delta.y;
                    float radiusSquared = menuButtonRadius * menuButtonRadius;

                    if (distanceSquared <= radiusSquared) {
                        switchToGameplay();
                    }
                }
            }
        }
    }
}

void GameStateManager::update() const {
    if (currentState == GameState::Gameplay && !entities.empty()) {
        // передаем указатель MC в PlayerControlSystem
        PlayerControlSystem::update(*entities[0], window);
    }
}

void GameStateManager::render() const {
    window.clear(sf::Color::Black);

    if (currentState == GameState::MainMenu) {
        sf::CircleShape dot(menuButtonRadius);
        dot.setFillColor(sf::Color::Cyan);
        dot.setPosition({menuButtonCenter.x - menuButtonRadius, menuButtonCenter.y - menuButtonRadius});
        window.draw(dot);
    } else if (currentState == GameState::Gameplay) {
        if (renderSystem) {
            RenderSystem::render(window, entities);
        }
    }

    window.display();
}
