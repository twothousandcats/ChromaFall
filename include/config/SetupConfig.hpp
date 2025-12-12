#pragma once
#include <SFML/System/Vector2.hpp>

// GENERAL
constexpr float HALF_DIVISOR = 2.f;

// SIZE
constexpr unsigned WINDOW_WIDTH = 800;
constexpr unsigned WINDOW_HEIGHT = 600;
constexpr sf::Vector2u WINDOW_SIZE = {WINDOW_WIDTH, WINDOW_HEIGHT};

// CENTER
constexpr unsigned int WINDOW_CENTER_X = WINDOW_WIDTH / HALF_DIVISOR;
constexpr unsigned int WINDOW_CENTER_Y = WINDOW_HEIGHT / HALF_DIVISOR;
constexpr sf::Vector2f WINDOW_CENTER = {WINDOW_CENTER_X, WINDOW_CENTER_Y};

// FPS
constexpr int FRAMES_PER_SECOND = 60;

// CLOCK
constexpr float DT_COMPENSATE_FACTOR = 0.1f;

// background
constexpr float WINDOW_SCROLL_SPEED = -20.f;