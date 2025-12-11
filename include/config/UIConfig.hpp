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
// state in game
const std::string STATE_VICTORY_TEXT = "VICTORY!";
constexpr sf::Color STATE_VICTORY_TEXT_COLOR = {0, 255, 200};
const std::string STATE_GAMEOVER_TEXT = "YOU DIED!";
constexpr sf::Color STATE_GAMEOVER_TEXT_COLOR = sf::Color::Red;
constexpr int STATE_TEXT_SIZE = 70;

// OVERLAYS
constexpr sf::Color OVERLAY_BGC = {39, 26, 80, 104}; // 271A50 40%
constexpr sf::Color OVERLAY_DESC_FIELD_BGC = {136, 123, 232, 150};
constexpr float OVERLAY_PADDING = 60.f;
constexpr float OVERLAY_TEXT_PADDING = 30.f;
constexpr sf::Vector2f OVERLAY_DESC_FIELD_POS = {OVERLAY_PADDING, OVERLAY_PADDING};
constexpr float OVERLAY_DESC_FIELD_WIDTH = WINDOW_WIDTH - OVERLAY_PADDING * 2.f;
constexpr float OVERLAY_DESC_FIELD_HEIGHT = 200.f;
constexpr sf::Vector2f OVERLAY_DESC_FIELD_SIZE = {OVERLAY_DESC_FIELD_WIDTH, OVERLAY_DESC_FIELD_HEIGHT};
constexpr sf::Vector2f OVERLAY_DESC_TEXT_POS = {OVERLAY_PADDING + OVERLAY_TEXT_PADDING, OVERLAY_PADDING + OVERLAY_TEXT_PADDING};

constexpr int OVERLAY_DESC_TEXT_FZ = 20;
constexpr sf::Color OVERLAY_DESC_TEXT_COLOR = sf::Color::White;

constexpr sf::Vector2f OPTION_SIZE = {80, 80};
constexpr int OVERLAY_UPGRADE_BTN_COUNT = 3;
constexpr int OVERLAY_UPGRADE_BTN_OUT_THICKNESS = 2;
constexpr float OVERLAY_UPGRADE_BTN_BASE_POS_X = OVERLAY_DESC_FIELD_WIDTH / 2.f - OPTION_SIZE.x;
constexpr float OVERLAY_UPGRADE_BTN_BASE_POS_Y = (OVERLAY_PADDING * 2.f) + OVERLAY_DESC_FIELD_HEIGHT + (OPTION_SIZE.y / 2.f) + OVERLAY_UPGRADE_BTN_OUT_THICKNESS;
constexpr sf::Vector2f OVERLAY_UPGRADE_BTN_BASE_POS = {OVERLAY_UPGRADE_BTN_BASE_POS_X, OVERLAY_UPGRADE_BTN_BASE_POS_Y};
constexpr float OVERLAY_UPGRADE_BTN_GAP = OVERLAY_PADDING + OPTION_SIZE.x;
constexpr sf::Color OVERLAY_UPGRADE_BTN_BGC = {0, 0, 0, 0};
constexpr sf::Color OVERLAY_UPGRADE_BTN_BOR_COLOR_NORMAL = {145, 138, 236, 255};
constexpr sf::Color OVERLAY_UPGRADE_BTN_BOR_COLOR_SELECTED = {0, 204, 109, 255};

constexpr int OVERLAY_UPGRADE_BTN_FZ = 20;
constexpr sf::Color OVERLAY_UPGRADE_BTN_FCOLOR = sf::Color::White;

constexpr float OVERLAY_MENU_WIDTH = OVERLAY_DESC_FIELD_WIDTH;
constexpr float OVERLAY_MENU_HEIGHT = 500.f;
constexpr sf::Vector2f OVERLAY_MENU_SIZE = {OVERLAY_MENU_WIDTH, OVERLAY_MENU_HEIGHT};
constexpr sf::Vector2f OVERLAY_MENU_POS = {OVERLAY_PADDING, OVERLAY_PADDING};

constexpr int OVERLAY_MENU_TITLE_FZ = 32;
constexpr sf::Color OVERLAY_MENU_TITLE_COLOR = sf::Color::White;
constexpr sf::Color OVERLAY_OPTION_NORMAL_COLOR = sf::Color::White;
constexpr sf::Vector2f OVERLAY_OPTION_BTN_SIZE = {300.f, 60.f};

// info
constexpr sf::Color INFO_TEXT_COLOR = sf::Color::White;

// BASE
const std::string BASE_FONT_REGULAR = "assets/fonts/Orbitron-Regular.ttf";
const std::string BASE_FONT_BOLD = "assets/fonts/Orbitron-Bold.ttf";
