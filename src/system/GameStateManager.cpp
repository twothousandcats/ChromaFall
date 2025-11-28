#include "systems/GameStateManager.hpp"

#include "entities/Entity.hpp"
#include "components/Position.hpp"
#include "systems/RenderSystem.hpp"

#include <SFML/Graphics.hpp>
#include <optional>

#include "config/GameConfig.hpp"
#include "config/SetupConfig.hpp"
#include "config/UIConfig.hpp"
#include "data/AsteroidTemplate.hpp"
#include "systems/GameSession.hpp"

GameStateManager::GameStateManager(sf::RenderWindow &window)
    : window(window) {
    // определяем шрифт, иначе exit
    if (!font.openFromFile(BASE_FONT_BOLD)) {
        if (!font.openFromFile(BASE_FONT_REGULAR)) {
            exit(1);
        }
    }

    // Заголовок
    sf::Text title(font);
    title.setString(TITLE_TEXT);
    title.setCharacterSize(TITLE_FONT_SIZE);
    title.setFillColor(TITLE_COLOR);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect bounds = title.getLocalBounds();
    title.setOrigin(bounds.size / HALF_DIVISOR);
    title.setPosition({WINDOW_CENTER_X, WINDOW_CENTER_Y - BUTTONS_Y_GAP});
    titleText = std::move(title);

    // Кнопки
    auto [startBg, startTxt] = createButton(
        START_BUTTON_TEXT,
        WINDOW_CENTER,
        font,
        BUTTON_FONT_SIZE,
        BUTTON_BG_COLOR,
        BUTTON_TEXT_COLOR
    );
    startButton = std::move(startBg);
    startText = std::move(startTxt);

    auto [exitBg, exitTxt] = createButton(
        EXIT_BUTTON_TEXT,
        {WINDOW_CENTER_X, WINDOW_HEIGHT / HALF_DIVISOR + BUTTONS_Y_GAP},
        font,
        BUTTON_FONT_SIZE,
        BUTTON_BG_COLOR,
        BUTTON_TEXT_COLOR
    );
    exitButton = std::move(exitBg);
    exitText = std::move(exitTxt);
}

GameStateManager::~GameStateManager() = default;

// решить вопрос с origin type
sf::Text GameStateManager::createText(
    const std::string &textContent,
    const sf::Vector2f &position,
    const sf::Font &font,
    const unsigned int charSize,
    const sf::Color &textColor,
    const sf::Text::Style &textStyle
) {
    sf::Text text(font);
    text.setString(textContent);
    text.setCharacterSize(charSize);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(textColor);
    text.setStyle(textStyle);

    const sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.size / HALF_DIVISOR);
    text.setPosition(position);

    return text;
}

std::pair<sf::RectangleShape, sf::Text> GameStateManager::createButton(
    const std::string &text,
    const sf::Vector2f &positionCenter,
    const sf::Font &font,
    const unsigned int charSize,
    const sf::Color &bgColor,
    const sf::Color &textColor
) {
    const sf::Text buttonText = createText(
        text,
        positionCenter,
        font,
        charSize,
        textColor
    );

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
            currentState = GameState::GameOver;
            infoClock.restart();
        } else if (gameSession->isVictory()) {
            currentState = GameState::Victory;
            infoClock.restart();
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

        // wave UI
        const sf::Text waveText = createText(
            "Wave: " + std::to_string(gameSession->getCurrentWave()) + " / " + std::to_string(
                gameSession->getTotalWaves()),
            {WINDOW_WIDTH - 130.f, WINDOW_HEIGHT - TITLE_FONT_SIZE},
            font,
            TITLE_FONT_SIZE,
            INFO_TEXT_COLOR,
            sf::Text::Regular
        );
        window.draw(waveText);

        const sf::Text hpText = createText(
            "HP: " + std::to_string(static_cast<int>(gameSession->getPlayerHp())),
            {70.f, WINDOW_HEIGHT - TITLE_FONT_SIZE},
            font,
            TITLE_FONT_SIZE,
            INFO_TEXT_COLOR,
            sf::Text::Regular
        );
        window.draw(hpText);
    } else if (currentState == GameState::Victory) {
        const sf::Text victoryText = createText(
            STATE_VICTORY_TEXT,
            WINDOW_CENTER,
            font,
            STATE_TEXT_SIZE,
            STATE_VICTORY_TEXT_COLOR,
            sf::Text::Bold
        );
        window.draw(victoryText);

        // Через 3 секунды — в меню
        if (infoClock.getElapsedTime().asSeconds() > TIME_TO_SHOW_STATUS) {
            switchToMainMenu();
            infoClock.restart();
        }
    } else if (currentState == GameState::GameOver) {
        const sf::Text gameLostText = createText(
            STATE_GAMEOVER_TEXT,
            WINDOW_CENTER,
            font,
            STATE_TEXT_SIZE,
            STATE_GAMEOVER_TEXT_COLOR,
            sf::Text::Bold
        );
        window.draw(gameLostText);

        // Через 3 секунды — в меню
        if (infoClock.getElapsedTime().asSeconds() > TIME_TO_SHOW_STATUS) {
            switchToMainMenu();
            infoClock.restart();
        }
    }

    window.display();
}
