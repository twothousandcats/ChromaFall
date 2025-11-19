#include "../include/systems/GameStateManager.hpp"

#include <iostream>

#include "../include/entities/Entity.hpp"
#include "../include/components/Position.hpp"
#include "../include/components/Renderable.hpp"
#include "../include/systems/RenderSystem.hpp"
#include "../include/systems/PlayerControlSystem.hpp"

#include <SFML/Graphics.hpp>
#include <optional>

constexpr unsigned int WINDOW_WIDTH = 800;
constexpr unsigned int WINDOW_HEIGHT = 600;
constexpr unsigned int WINDOW_CENTER_X = WINDOW_WIDTH / 2.f;
constexpr sf::Vector2f WINDOW_SIZE = {WINDOW_CENTER_X, WINDOW_HEIGHT / 2.f};

GameStateManager::GameStateManager(sf::RenderWindow &window)
    : window(window), currentState(GameState::MainMenu) {
    // определяем шрифт, иначе exit
    if (!font.openFromFile(boldFont)) {
        exit(1);
    }

    // Заголовок
    sf::Text title(font);
    title.setString(gameTitle);
    title.setCharacterSize(titleFontSize);
    title.setFillColor(titleColor);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect bounds = title.getLocalBounds();
    title.setOrigin(bounds.size / 2.f);
    title.setPosition({WINDOW_CENTER_X, WINDOW_HEIGHT / 2.f - buttonGap});
    titleText = std::move(title);

    // Кнопки
    auto [startBg, startTxt] = createButton(
        startTextValue,
        {WINDOW_CENTER_X, WINDOW_HEIGHT / 2.f},
        font,
        buttonFontSize,
        buttonColor,
        buttonTextColor
    );
    startButton = std::move(startBg);
    startText = std::move(startTxt);

    auto [exitBg, exitTxt] = createButton(
        exitTextValue,
        {WINDOW_CENTER_X, WINDOW_HEIGHT / 2.f + buttonGap},
        font,
        buttonFontSize,
        buttonColor,
        buttonTextColor
    );
    exitButton = std::move(exitBg);
    exitText = std::move(exitTxt);
}

GameStateManager::~GameStateManager() = default;

std::pair<sf::RectangleShape, sf::Text> GameStateManager::createButton(
    const std::string &text,
    const sf::Vector2f &positionCenter,
    const sf::Font &font,
    const unsigned int charSize,
    const sf::Color &bgColor,
    const sf::Color &textColor
) {
    sf::Text buttonText(font);
    buttonText.setString(text);
    buttonText.setCharacterSize(charSize);
    buttonText.setStyle(sf::Text::Bold);
    buttonText.setFillColor(textColor);
    const sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setOrigin(textBounds.size / 2.f);
    buttonText.setPosition(positionCenter);

    sf::RectangleShape buttonBg;
    buttonBg.setSize({200.f, 60.f});
    buttonBg.setFillColor(bgColor);
    buttonBg.setPosition({
        positionCenter.x - buttonBg.getSize().x / 2.f,
        positionCenter.y - buttonBg.getSize().y / 2.f
    });

    return {buttonBg, buttonText};
}

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
    bullets.clear();

    // создаем MC
    auto player = std::make_unique<Entity>();
    player->addComponent(std::make_unique<Position>());
    player->addComponent(std::make_unique<Renderable>());
    entities.push_back(std::move(player));

    renderSystem = new RenderSystem();
    playerControlSystem = new PlayerControlSystem();
}

void GameStateManager::handleEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        } else if (currentState == GameState::MainMenu) {
            if (event->is<sf::Event::MouseButtonPressed>()) {
                const auto &mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::FloatRect startButtonBounds = startButton.getGlobalBounds();
                    sf::FloatRect exitButtonBounds = exitButton.getGlobalBounds();
                    if (startButtonBounds.contains(static_cast<sf::Vector2f>(mousePos))) {
                        switchToGameplay();
                    }
                    if (exitButtonBounds.contains(static_cast<sf::Vector2f>(mousePos))) {
                        window.close();
                    }
                }
            }
        } else if (event->is<sf::Event::KeyPressed>()) {
            const auto &keyEvent = event->getIf<sf::Event::KeyPressed>();
            if (keyEvent->code == sf::Keyboard::Key::Space && currentState == GameState::MainMenu) {
                switchToGameplay();
            }
        }
    }
}

void GameStateManager::update() {
    float deltaTime = gameClock.restart().asSeconds();

    if (deltaTime > 0.1f) {
        deltaTime = 0.1f;
    }

    if (currentState == GameState::Gameplay && !entities.empty()) {
        // передаем указатель MC в PlayerControlSystem
        PlayerControlSystem::update(*entities[0], window);

        // система стрельбы
        auto *playerPosition = entities[0]->getComponent<Position>();
        if (playerPosition) {
            shootSystem.update(
                bullets,
                window,
                playerPosition->value,
                bulletsCount,
                bulletsSpreadAngle
            );
        }

        // определение системы движения (пули)
        movementSystem.update(bullets, deltaTime);

        std::cout << "Пуль в игре: " << bullets.size() << std::endl;

        // удаление
        for (auto it = bullets.begin(); it != bullets.end();) {
            auto *position = (*it)->getComponent<Position>();
            if (!position || position->value.y < -20.f) {
                it = bullets.erase(it);
            } else {
                ++it;
            }
        }

        /* bool isLeftPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        static bool lastState = false;
        if (isLeftPressed != lastState) {
            std::cout << "Удержание ЛКМ: " << (isLeftPressed ? "ВКЛ" : "ВЫКЛ") << std::endl;
            lastState = isLeftPressed;
        } */
    }
}

void GameStateManager::render() {
    window.clear(sf::Color::Black);

    if (currentState == GameState::MainMenu) {
        if (titleText) window.draw(*titleText);
        window.draw(startButton);
        if (startText) window.draw(*startText);
        window.draw(exitButton);
        if (exitText) window.draw(*exitText);
    } else if (currentState == GameState::Gameplay) {
        // MC
        if (renderSystem && !entities.empty()) {
            RenderSystem::render(window, entities);
        }

        // пули (RenderSystem временный чисто для рендера)
        if (!bullets.empty()) {
            RenderSystem bulletsRender;
            bulletsRender.render(window, bullets);
        }
    }

    window.display();
}
