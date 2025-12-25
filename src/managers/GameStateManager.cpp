#include "managers/GameStateManager.hpp"

#include "entities/Entity.hpp"
#include "components/Position.hpp"
#include "systems/RenderSystem.hpp"

#include <SFML/Graphics.hpp>
#include <optional>

#include "config/GameConfig.hpp"
#include "config/SetupConfig.hpp"
#include "config/UIConfig.hpp"
#include "data/AsteroidTemplate.hpp"
#include "data/TextOrigin.hpp"
#include "managers/AudioManager.hpp"
#include "managers/GameSession.hpp"

GameStateManager::GameStateManager(sf::RenderWindow &window)
    : window(window), currentState(GameState::MainMenu) {
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

    if (hpTexture.loadFromFile(BASE_HEART)) {
        hpSprite.emplace(hpTexture);
    } else {
        exit(1);
    }

    // явно (музыка)
    switchToMainMenu();

    // загрузка звуков
    auto &audio = AudioManager::getInstance();

    // Shooting
    audio.loadSound(AUDIO_SFX_BLASTER_SHOOT_NAME, AUDIO_SFX_BLASTER_SHOOT_PATH);
    audio.loadSound(AUDIO_SFX_SHOTGUN_SHOOT_NAME, AUDIO_SFX_SHOTGUN_SHOOT_PATH);
    audio.loadSound(AUDIO_SFX_LASER_SHOOT_NAME, AUDIO_SFX_LASER_SHOOT_PATH);

    // ui
    audio.loadSound(AUDIO_SFX_SELECT_BUTTON_NAME, AUDIO_SFX_SELECT_BUTTON_PATH);
    audio.loadSound(AUDIO_SFX_SELECT_UPGRADE_NAME, AUDIO_SFX_SELECT_UPGRADE_PATH);
    audio.loadSound(AUDIO_SFX_LVLUP_NAME, AUDIO_SFX_LVLUP_PATH);
}

GameStateManager::~GameStateManager() = default;

sf::Text GameStateManager::createText(
    const std::string &textContent,
    const sf::Vector2f &position,
    const sf::Font &font,
    const unsigned int charSize,
    const sf::Color &textColor,
    const sf::Text::Style &textStyle,
    const TextOrigin origin
) {
    sf::Text text(font);
    text.setString(textContent);
    text.setCharacterSize(charSize);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(textColor);
    text.setStyle(textStyle);

    const sf::FloatRect textBounds = text.getLocalBounds();
    sf::Vector2f originPoint;
    switch (origin) {
        case TextOrigin::TOP_LEFT:
            originPoint = {0.f, 0.f};
            break;
        case TextOrigin::TOP_RIGHT:
            originPoint = {textBounds.size.x, 0};
            break;
        case TextOrigin::BOTTOM_LEFT:
            originPoint = {0.f, textBounds.size.y};
            break;
        case TextOrigin::BOTTOM_RIGHT:
            originPoint = {textBounds.size.x, textBounds.size.y};
            break;
        case TextOrigin::CENTER:
            originPoint = textBounds.size / 2.f;
            break;
    }
    text.setOrigin(originPoint);
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

void GameStateManager::renderPowerUpOverlay(
    sf::RenderWindow &window
) {
    const auto &options = gameSession->getPowerUpOptions();
    int selectedIndex = gameSession->getSelectedPowerUpIndex();

    // Описания
    const std::vector<std::string> descriptions = {
        "Increases 1 max health points",
        "Increases weapon damage by 30%",
        "Increases number of bullets per shot",
        "Increases attack speed by 25%",
        "Unlocks Blaster",
        "Unlocks shotgun",
        "Unlocks laser"
    };
    // Усиления
    const std::vector<std::string> labels = {
        "+HP",
        "+DMG",
        "+BLT",
        "+ASPD",
        "Blaster",
        "Shotgun",
        "Laser"
    };

    // рендер descField
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(options.size())) {
        PowerUpType selectedType = options[selectedIndex];
        std::string descText = descriptions[static_cast<int>(selectedType)];

        sf::RectangleShape desc(OVERLAY_DESC_FIELD_SIZE);
        desc.setPosition(OVERLAY_DESC_FIELD_POS);
        desc.setFillColor(OVERLAY_DESC_FIELD_BGC);
        window.draw(desc);

        sf::Text description = createText(
            descText,
            OVERLAY_DESC_TEXT_POS,
            font,
            OVERLAY_DESC_TEXT_FZ,
            OVERLAY_DESC_TEXT_COLOR,
            sf::Text::Bold,
            TextOrigin::TOP_LEFT
        );
        window.draw(description);
    }

    // варианты
    for (size_t i = 0; i < options.size(); ++i) {
        sf::Color outlineColor = (i == static_cast<size_t>(selectedIndex))
                                     ? OVERLAY_UPGRADE_BTN_BOR_COLOR_SELECTED
                                     : OVERLAY_UPGRADE_BTN_BOR_COLOR_NORMAL;

        sf::RectangleShape cell(OPTION_SIZE);
        cell.setPosition({
            OVERLAY_UPGRADE_BTN_BASE_POS_X + (OVERLAY_UPGRADE_BTN_GAP * i),
            OVERLAY_UPGRADE_BTN_BASE_POS_Y
        });
        cell.setFillColor(OVERLAY_UPGRADE_BTN_BGC);
        cell.setOutlineColor(outlineColor);
        cell.setOutlineThickness(OVERLAY_UPGRADE_BTN_OUT_THICKNESS);
        const sf::FloatRect cellBounds = cell.getLocalBounds();
        cell.setOrigin(cellBounds.size / 2.f);
        cell.setRotation(sf::degrees(45.f));
        window.draw(cell);

        sf::Text label = createText(
            labels[static_cast<int>(options[i])],
            {OVERLAY_UPGRADE_BTN_BASE_POS_X + (OVERLAY_UPGRADE_BTN_GAP * i), OVERLAY_UPGRADE_BTN_BASE_POS_Y},
            font,
            OVERLAY_UPGRADE_BTN_FZ,
            OVERLAY_UPGRADE_BTN_FCOLOR,
            sf::Text::Bold,
            TextOrigin::CENTER
        );
        window.draw(label);
    }

    // hint
    if (static_cast<int>(blinkClock.getElapsedTime().asSeconds() * 2) % 2 == 0) {
        const sf::Text hint = createText(
            "PRESS SPACE TO CONFIRM",
            {WINDOW_CENTER_X, WINDOW_HEIGHT - static_cast<float>(OVERLAY_DESC_TEXT_FZ) - OVERLAY_PADDING},
            font,
            OVERLAY_DESC_TEXT_FZ,
            OVERLAY_UPGRADE_BTN_FCOLOR,
            sf::Text::Bold
        );
        window.draw(hint);
    }
}

void GameStateManager::renderPauseOverlay(
    sf::RenderWindow &window
) {
    sf::RectangleShape descOverlay(OVERLAY_MENU_SIZE);
    descOverlay.setPosition(OVERLAY_MENU_POS);
    descOverlay.setFillColor(OVERLAY_DESC_FIELD_BGC);
    window.draw(descOverlay);

    sf::Text title = createText(
        "Pause",
        {WINDOW_CENTER_X, OVERLAY_MENU_POS.y + OVERLAY_PADDING},
        font,
        OVERLAY_MENU_TITLE_FZ,
        OVERLAY_MENU_TITLE_COLOR,
        sf::Text::Bold
    );
    window.draw(title);

    // Параметры кнопок
    const sf::Vector2f firstButtonPos = {
        WINDOW_CENTER_X,
        OVERLAY_MENU_POS.y + OVERLAY_PADDING + OVERLAY_MENU_TITLE_FZ + (BASE_GAP + OVERLAY_OPTION_BTN_SIZE.y)
    };

    // Варианты паузы
    std::vector<std::pair<std::string, PauseOption> > options = {
        {"Resume", PauseOption::RESUME},
        {"Main Menu", PauseOption::MAIN_MENU}
    };

    for (size_t i = 0; i < options.size(); ++i) {
        const auto &[label, option] = options[i];
        sf::Vector2f pos = {
            firstButtonPos.x,
            firstButtonPos.y + static_cast<float>(i) * (BASE_GAP + OVERLAY_OPTION_BTN_SIZE.y)
        };
        bool isSelected = (selectedPauseOption == option);

        sf::Color fillColor = isSelected
                                  ? OVERLAY_UPGRADE_BTN_BOR_COLOR_SELECTED
                                  : sf::Color::Transparent;

        // Фон кнопки
        sf::RectangleShape buttonBg;
        sf::Text buttonText = createText(
            label,
            pos,
            font,
            OVERLAY_UPGRADE_BTN_FZ,
            OVERLAY_OPTION_NORMAL_COLOR,
            sf::Text::Regular,
            TextOrigin::CENTER
        );

        buttonBg.setSize(OVERLAY_OPTION_BTN_SIZE);
        buttonBg.setFillColor(fillColor);
        buttonBg.setOutlineColor(OVERLAY_UPGRADE_BTN_BOR_COLOR_SELECTED);
        buttonBg.setOutlineThickness(OVERLAY_UPGRADE_BTN_OUT_THICKNESS);
        buttonBg.setPosition({pos.x - buttonBg.getSize().x / 2.f, pos.y - buttonBg.getSize().y / 2.f});

        window.draw(buttonBg);
        window.draw(buttonText);
    }
}

void GameStateManager::switchToMainMenu() {
    currentState = GameState::MainMenu;
    gameSession.reset(); // отчистка

    if (isGameMusicPlaying) {
        AudioManager::getInstance().stopMusic();
        isGameMusicPlaying = false;
    }

    if (!isThemeMusicPlaying) {
        AudioManager::getInstance().playMusic(AUDIO_MUSIC_THEME);
        isThemeMusicPlaying = true;
    }
}

void GameStateManager::switchToGameplay() {
    currentState = GameState::Gameplay;
    gameSession = std::make_unique<GameSession>(window);

    if (isThemeMusicPlaying) {
        AudioManager::getInstance().stopMusic();
        isThemeMusicPlaying = false;
    }

    if (!isGameMusicPlaying) {
        AudioManager::getInstance().playMusic(AUDIO_MUSIC_SESSION);
        isGameMusicPlaying = true;
    }
}

void GameStateManager::handleMainMenuEvents(const sf::Event &event) {
    if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto &mouseEv = *event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEv.button == sf::Mouse::Button::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (startButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                AudioManager::getInstance().playSound(AUDIO_SFX_SELECT_BUTTON_NAME);
                switchToGameplay();
            } else if (exitButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                window.close();
            }
        }
    } else if (event.is<sf::Event::KeyPressed>()) {
        const auto &keyEv = *event.getIf<sf::Event::KeyPressed>();
        if (keyEv.code == sf::Keyboard::Key::Space) {
            AudioManager::getInstance().playSound(AUDIO_SFX_SELECT_BUTTON_NAME);
            switchToGameplay();
        }
    }
}

void GameStateManager::handlePowerUpSelectionInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Key::Left || key == sf::Keyboard::Key::A) {
        gameSession->selectPrevPowerUp();
        AudioManager::getInstance().playSound(AUDIO_SFX_SELECT_UPGRADE_NAME);
    } else if (key == sf::Keyboard::Key::Right || key == sf::Keyboard::Key::D) {
        gameSession->selectNextPowerUp();
        AudioManager::getInstance().playSound(AUDIO_SFX_SELECT_UPGRADE_NAME);
    } else if (key == sf::Keyboard::Key::Space || key == sf::Keyboard::Key::Enter) {
        gameSession->confirmPowerUpSelection();
    }
}

void GameStateManager::handlePauseMenuInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Key::Up || key == sf::Keyboard::Key::W) {
        selectedPauseOption = PauseOption::RESUME;
        AudioManager::getInstance().playSound(AUDIO_SFX_SELECT_BUTTON_NAME);
    } else if (key == sf::Keyboard::Key::Down || key == sf::Keyboard::Key::S) {
        selectedPauseOption = PauseOption::MAIN_MENU;
        AudioManager::getInstance().playSound(AUDIO_SFX_SELECT_BUTTON_NAME);
    } else if (key == sf::Keyboard::Key::Space || key == sf::Keyboard::Key::Enter) {
        if (selectedPauseOption == PauseOption::RESUME) {
            gameSession->setOverlayState(OverlayState::NONE);
            AudioManager::getInstance().resumeMusic();
        } else {
            switchToMainMenu();
        }
    }
}

void GameStateManager::handleGameplayEvents(const sf::Event &event) {
    if (!gameSession) return;

    auto overlay = gameSession->getOverlayState();

    if (event.is<sf::Event::KeyPressed>()) {
        const auto &key = event.getIf<sf::Event::KeyPressed>()->code;

        if (key == sf::Keyboard::Key::Escape) {
            if (overlay == OverlayState::PAUSE) {
                gameSession->setOverlayState(OverlayState::NONE);
                AudioManager::getInstance().resumeMusic();
            } else {
                gameSession->setOverlayState(OverlayState::PAUSE);
                selectedPauseOption = PauseOption::RESUME;
                AudioManager::getInstance().pauseMusic();
            }
            return;
        }

        if (overlay == OverlayState::POWER_UP_SELECTION) {
            handlePowerUpSelectionInput(key);
        } else if (overlay == OverlayState::PAUSE) {
            handlePauseMenuInput(key);
        }
    }
}

// states
void GameStateManager::handleEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return;
        }

        switch (currentState) {
            case GameState::MainMenu:
                handleMainMenuEvents(*event);
                break;
            case GameState::Gameplay:
                handleGameplayEvents(*event);
                break;
        }
    }
}

void GameStateManager::updateGameplay(float deltaTime) {
    if (!gameSession) return;
    gameSession->update(deltaTime);

    if (gameSession->isGameOver()) {
        currentState = GameState::GameOver;
        infoClock.restart();
    } else if (gameSession->isVictory()) {
        currentState = GameState::Victory;
        infoClock.restart();
    }
}

void GameStateManager::updateResultScreen() {
    if (infoClock.getElapsedTime().asSeconds() > TIME_TO_SHOW_STATUS) {
        switchToMainMenu();
        infoClock.restart();
    }
}

void GameStateManager::update() {
    float deltaTime = gameClock.restart().asSeconds();
    if (deltaTime > DT_COMPENSATE_FACTOR) {
        deltaTime = DT_COMPENSATE_FACTOR;
    }

    switch (currentState) {
        case GameState::Gameplay:
            updateGameplay(deltaTime);
            break;
        case GameState::Victory:
        case GameState::GameOver:
            updateResultScreen();
            break;
        default:
            break;
    }
}

void GameStateManager::renderMainMenu() {
    if (titleText) window.draw(*titleText);
    window.draw(startButton);
    if (startText) window.draw(*startText);
    window.draw(exitButton);
    if (exitText) window.draw(*exitText);
}

void GameStateManager::renderGameplay() {
    if (!gameSession) return;

    gameSession->render(window);
    renderGameplayHUD();
    renderGameplayOverlays();
}

void GameStateManager::renderGameplayHUD() {
    // Wave
    window.draw(createText(
        "Wave: " + std::to_string(gameSession->getCurrentWave()) + " / " +
        std::to_string(gameSession->getTotalWaves()),
        INFO_WAVE_POS, font, INFO_FZ, INFO_TEXT_COLOR,
        sf::Text::Regular, TextOrigin::BOTTOM_RIGHT
    ));

    // Kills
    window.draw(createText(
        "Kills: " + std::to_string(gameSession->getCurrentWaveKilledCount()) + " / " +
        std::to_string(gameSession->getCurrentWaveTotalCount()),
        INFO_WAVE_KILLS_POS, font, INFO_FZ, INFO_TEXT_COLOR,
        sf::Text::Regular, TextOrigin::BOTTOM_RIGHT
    ));

    // Level
    window.draw(createText(
        "Lvl: " + std::to_string(gameSession->getPlayerLevel()),
        INFO_LEVEL_POS, font, INFO_FZ, INFO_TEXT_COLOR,
        sf::Text::Regular, TextOrigin::BOTTOM_RIGHT
    ));

    // XP
    window.draw(createText(
        "XP: " + std::to_string(gameSession->getPlayerCurrentExp()) + " / " +
        std::to_string(gameSession->getPlayerExpForNextLevel()),
        INFO_EXP_POS, font, INFO_FZ, INFO_TEXT_COLOR,
        sf::Text::Regular, TextOrigin::BOTTOM_RIGHT
    ));

    // HP
    if (hpSprite) {
        const auto spriteSize = static_cast<float>(hpSprite->getTextureRect().size.y);
        hpSprite->setPosition({INFO_HP_POS_X, INFO_HP_POS_Y});
        hpSprite->setOrigin({0.f, spriteSize});
        window.draw(*hpSprite);

        auto hpText = createText(
            std::to_string(static_cast<int>(gameSession->getPlayerHp())),
            {INFO_HP_POS_X + INFO_PADDING / 2.f + spriteSize, INFO_HP_POS_Y - INFO_FZ / 2.f},
            font, INFO_FZ, INFO_TEXT_COLOR,
            sf::Text::Regular, TextOrigin::BOTTOM_LEFT
        );
        window.draw(hpText);

        sf::RectangleShape hpBar;
        float w = spriteSize + hpText.getLocalBounds().size.x + INFO_PADDING * 2.f + INFO_FZ / 2.f;
        float h = INFO_FZ + INFO_PADDING * 2.f;
        hpBar.setPosition({0.f, WINDOW_HEIGHT - h});
        hpBar.setSize({w, h});
        hpBar.setFillColor(sf::Color::Transparent);
        hpBar.setOutlineColor(INFO_HP_BORDER_COLOR);
        hpBar.setOutlineThickness(INFO_HP_BORDER_WIDTH);
        window.draw(hpBar);
    }
}

void GameStateManager::renderGameplayOverlays() {
    if (!gameSession || gameSession->getOverlayState() == OverlayState::NONE) {
        return;
    }

    sf::RectangleShape overlayBg({static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)});
    overlayBg.setFillColor(OVERLAY_BGC);
    window.draw(overlayBg);

    switch (gameSession->getOverlayState()) {
        case OverlayState::POWER_UP_SELECTION:
            renderPowerUpOverlay(window);
            break;
        case OverlayState::PAUSE:
            renderPauseOverlay(window);
            break;
        default:
            break;
    }
}

void GameStateManager::renderVictory() {
    window.draw(createText(
        STATE_VICTORY_TEXT, WINDOW_CENTER, font, STATE_TEXT_SIZE,
        STATE_VICTORY_TEXT_COLOR, sf::Text::Bold
    ));
}

void GameStateManager::renderGameOver() {
    window.draw(createText(
        STATE_GAMEOVER_TEXT, WINDOW_CENTER, font, STATE_TEXT_SIZE,
        STATE_GAMEOVER_TEXT_COLOR, sf::Text::Bold
    ));
}

void GameStateManager::render() {
    window.clear(sf::Color::Black);

    switch (currentState) {
        case GameState::MainMenu:
            renderMainMenu();
            break;
        case GameState::Gameplay:
            renderGameplay();
            break;
        case GameState::Victory:
            renderVictory();
            break;
        case GameState::GameOver:
            renderGameOver();
            break;
    }

    window.display();
}
