#include "../include/systems/GameStateManager.hpp"

#include <iostream>

#include "../include/entities/Entity.hpp"
#include "../include/components/Position.hpp"
#include "../include/components/Renderable.hpp"
#include "../include/systems/RenderSystem.hpp"
#include "../include/systems/PlayerControlSystem.hpp"

#include <SFML/Graphics.hpp>
#include <optional>

#include "config/SetupConfig.hpp"
#include "config/UIConfig.hpp"
#include "data/AsteroidTemplate.hpp"
#include "systems/GameSession.hpp"

GameStateManager::GameStateManager(sf::RenderWindow &window)
    : window(window) {
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
    title.setOrigin(bounds.size / HALF_DIVISOR);
    title.setPosition({WINDOW_CENTER_X, WINDOW_CENTER_Y - buttonGap});
    titleText = std::move(title);

    // Кнопки
    auto [startBg, startTxt] = createButton(
        startTextValue,
        WINDOW_CENTER,
        font,
        buttonFontSize,
        buttonColor,
        buttonTextColor
    );
    startButton = std::move(startBg);
    startText = std::move(startTxt);

    auto [exitBg, exitTxt] = createButton(
        exitTextValue,
        {WINDOW_CENTER_X, WINDOW_HEIGHT / HALF_DIVISOR + buttonGap},
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
    buttonText.setOrigin(textBounds.size / HALF_DIVISOR);
    buttonText.setPosition(positionCenter);

    sf::RectangleShape buttonBg;
    buttonBg.setSize(BUTTON_SIZE);
    buttonBg.setFillColor(bgColor);
    buttonBg.setPosition({
        positionCenter.x - buttonBg.getSize().x / HALF_DIVISOR,
        positionCenter.y - buttonBg.getSize().y / HALF_DIVISOR
    });

    return {buttonBg, buttonText};
}

void GameStateManager::switchToMainMenu() {
    currentState = GameState::MainMenu;
    gameSession.reset(); // отчистка
}

void GameStateManager::switchToGameplay() {
    currentState = GameState::Gameplay;
    gameSession = std::make_unique<GameSession>(window);
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

    if (deltaTime > DT_COMPENSATE_FACTOR) {
        deltaTime = DT_COMPENSATE_FACTOR;
    }

    if (currentState == GameState::Gameplay && gameSession) {
        gameSession->update(deltaTime);
        if (gameSession->isGameOver()) {
            currentState = GameState::GameOver; // todo: реализовать экран
        }
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
    } else if (currentState == GameState::Gameplay && gameSession) {
        gameSession->render(window);
    } else if (currentState == GameState::GameOver) {
        window.close(); // todo: реализовать экран
    }

    window.display();
}
