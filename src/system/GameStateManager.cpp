#include "../include/systems/GameStateManager.hpp"

#include <iostream>

#include "../include/entities/Entity.hpp"
#include "../include/components/Position.hpp"
#include "../include/components/Renderable.hpp"
#include "../include/systems/RenderSystem.hpp"
#include "../include/systems/PlayerControlSystem.hpp"

#include <SFML/Graphics.hpp>
#include <optional>

#include "components/Acceleration.hpp"
#include "components/Asteroid.hpp"
#include "components/Health.hpp"
#include "components/Velocity.hpp"

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

void GameStateManager::spawnAsteroid() {
    // TODO: Вынести в конфиг
    auto asteroid = std::make_unique<Entity>();
    // определим х
    auto x = static_cast<float>(rand() % WINDOW_WIDTH);
    asteroid->addComponent(std::make_unique<Position>(x, -30.f));
    asteroid->addComponent(std::make_unique<Velocity>());
    asteroid->addComponent(std::make_unique<Acceleration>(0.f, 50.f));
    asteroid->addComponent(std::make_unique<Asteroid>(AsteroidSize::LARGE));
    asteroid->addComponent(std::make_unique<Health>(3.f));
    asteroid->addComponent(std::make_unique<Renderable>(
        60.f,
        60.f,
        sf::Color::White
    ));

    asteroids.push_back(std::move(asteroid));
}

bool intersects(
    const sf::Vector2f &aPos,
    const sf::Vector2f &bPos,
    const sf::Vector2f &aSize,
    const sf::Vector2f &bSize
) {
    const float aLeft = aPos.x - aSize.x / 2.f;
    const float aRight = aPos.x + aSize.x / 2.f;
    const float aTop = aPos.y - aSize.y / 2.f;
    const float aBottom = aPos.y + aSize.y / 2.f;

    const float bLeft = bPos.x - bSize.x / 2.f;
    const float bRight = bPos.x + bSize.x / 2.f;
    const float bTop = bPos.y - bSize.y / 2.f;
    const float bBottom = bPos.y + bSize.y / 2.f;

    return aRight > bLeft
           && aLeft < bRight
           && aBottom > bTop
           && aTop < bBottom;
}

void GameStateManager::checkCollisions() {
    // bullet -> asteroid
    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
        auto *bulletPos = (*bulletIt)->getComponent<Position>();
        auto *bulletRender = (*bulletIt)->getComponent<Renderable>();
        bool bulletHit = false;

        if (bulletPos && bulletRender) {
            sf::Vector2f bulletSize = bulletRender->shape.getSize();

            for (auto asteroidIt = asteroids.begin(); asteroidIt != asteroids.end();) {
                auto *asteroidPos = (*asteroidIt)->getComponent<Position>();
                auto *asteroidRender = (*asteroidIt)->getComponent<Renderable>();
                auto *asteroidHealth = (*asteroidIt)->getComponent<Health>();

                if (asteroidPos && asteroidRender && asteroidHealth) {
                    sf::Vector2f asteroidSize = asteroidRender->shape.getSize();
                    // попадание
                    if (intersects(
                        bulletPos->value,
                        asteroidPos->value,
                        bulletSize,
                        asteroidSize
                    )) {
                        asteroidHealth->value -= 1.f;
                        bulletHit = true;

                        if (asteroidHealth->value <= 0.f) {
                            // TODO: split *0.4hp
                            asteroidIt = asteroids.erase(asteroidIt);
                        } else {
                            ++asteroidIt;
                        }
                        break;
                    } else {
                        ++asteroidIt;
                    }
                } else {
                    ++asteroidIt;
                }
            }
        }

        if (bulletHit) {
            bulletIt = bullets.erase(bulletIt);
        } else {
            ++bulletIt;
        }
    }

    // asteroid -> player
    if (player) {
        auto *playerPos = player->getComponent<Position>();
        auto *playerRender = player->getComponent<Renderable>();
        if (playerPos && playerRender) {
            sf::Vector2f playerSize = playerRender->shape.getSize();
            auto *playerHealth = player->getComponent<Health>();
            for (auto asteroidIt = asteroids.begin(); asteroidIt != asteroids.end();) {
                auto *asteroidPos = (*asteroidIt)->getComponent<Position>();
                auto *asteroidRender = (*asteroidIt)->getComponent<Renderable>();
                if (asteroidPos && asteroidRender) {
                    sf::Vector2f asteroidSize = asteroidRender->shape.getSize();
                    if (intersects(
                        playerPos->value,
                        asteroidPos->value,
                        playerSize,
                        asteroidSize
                    )) {
                        asteroidIt = asteroids.erase(asteroidIt);
                        playerHealth->value -= 1.f;
                        std::cout << "player hit!" << playerHealth->value << std::endl;
                    } else {
                        ++asteroidIt;
                    }
                } else {
                    ++asteroidIt;
                }
            }
        }
    }
}

void GameStateManager::switchToMainMenu() {
    currentState = GameState::MainMenu;

    // очищаем сущности
    player.reset();
    bullets.clear();
}

void GameStateManager::switchToGameplay() {
    currentState = GameState::Gameplay;
    bullets.clear();

    // создаем MC
    player = std::make_unique<Entity>();
    player->addComponent(std::make_unique<Position>());
    player->addComponent(std::make_unique<Renderable>());
    player->addComponent(std::make_unique<Health>(3.f));
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

    if (currentState == GameState::Gameplay && player) {
        // передаем указатель MC в PlayerControlSystem
        PlayerControlSystem::update(*player, window);

        // система стрельбы
        auto *playerPosition = player->getComponent<Position>();
        if (playerPosition) {
            shootSystem.update(
                bullets,
                window,
                playerPosition->value,
                bulletsCount,
                bulletsSpreadAngle
            );
        }

        // система движения (пули)
        movementSystem.update(bullets, deltaTime);

        if (asteroidClock.getElapsedTime().asSeconds() > 1.f) {
            spawnAsteroid();
            asteroidClock.restart();
        }

        movementSystem.update(asteroids, deltaTime);

        std::cout << "Пуль: " << bullets.size() << std::endl;

        // TODO: в процедуру удаления
        // удаление астероидов
        for (auto it = asteroids.begin(); it != asteroids.end();) {
            auto *position = (*it)->getComponent<Position>();
            if (!position || position->value.y > static_cast<float>(window.getSize().y) + 20.f) {
                it = asteroids.erase(it);
            } else {
                ++it;
            }
        }

        // удаление пуль
        for (auto it = bullets.begin(); it != bullets.end();) {
            auto *position = (*it)->getComponent<Position>();
            if (!position || position->value.y < -20.f) {
                it = bullets.erase(it);
            } else {
                ++it;
            }
        }

        checkCollisions();
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
        if (player) {
            RenderSystem::render(window, {player.get()});
        }

        // пули
        if (!bullets.empty()) {
            std::vector<Entity *> bulletsPtrs;
            bulletsPtrs.reserve(bullets.size());
            for (const auto &bullet: bullets) {
                bulletsPtrs.push_back(bullet.get());
            }
            RenderSystem::render(window, bulletsPtrs);
        }

        if (!asteroids.empty()) {
            std::vector<Entity *> asteroidsPtrs;
            asteroidsPtrs.reserve(asteroids.size());
            for (const auto &asteroid: asteroids) {
                asteroidsPtrs.push_back(asteroid.get());
            }
            RenderSystem::render(window, asteroidsPtrs);
        }
    }

    window.display();
}
