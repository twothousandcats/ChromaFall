#pragma once

#include "SFML/Graphics.hpp"

// TITLE
// typograph
constexpr sf::Color TITLE_COLOR = {147, 30, 119, 255};
constexpr int TITLE_FONT_SIZE = 40;
const std::string TITLE_TEXT = "ChromaFall";

// BUTTON
// block
constexpr float BASE_GAP = 40.f;
constexpr float BUTTON_WIDTH = 200.f;
constexpr float BUTTON_HEIGHT = 60.f;
constexpr sf::Vector2f BUTTON_SIZE = {BUTTON_WIDTH, BUTTON_HEIGHT};
constexpr float BUTTONS_Y_GAP = BASE_GAP + BUTTON_HEIGHT;

// colors
constexpr sf::Color BUTTON_BG_COLOR = {0, 204, 109, 255};
constexpr sf::Color BUTTON_TEXT_COLOR = {255, 255, 255, 255};

// typograph
constexpr int BUTTON_FONT_SIZE = 32;
const std::string START_BUTTON_TEXT = "Start";
const std::string EXIT_BUTTON_TEXT = "Exit";

// GAMEPLAY
// state
const std::string STATE_VICTORY_TEXT = "VICTORY!";
constexpr sf::Color STATE_VICTORY_TEXT_COLOR = {0, 255, 200};
const std::string STATE_GAMEOVER_TEXT = "YOU DIED!";
constexpr sf::Color STATE_GAMEOVER_TEXT_COLOR = sf::Color::Red;
constexpr int STATE_TEXT_SIZE = 70;

// info
constexpr sf::Color INFO_TEXT_COLOR = sf::Color::White;

// BASE
const std::string BASE_FONT_REGULAR = "assets/fonts/Orbitron-Regular.ttf";
const std::string BASE_FONT_BOLD = "assets/fonts/Orbitron-Bold.ttf";