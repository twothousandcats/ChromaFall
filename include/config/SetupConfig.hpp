#pragma once
#include <SFML/System/Vector2.hpp>

// SIZE
constexpr unsigned WINDOW_WIDTH = 800;
constexpr unsigned WINDOW_HEIGHT = 600;
constexpr sf::Vector2u WINDOW_SIZE = {WINDOW_WIDTH, WINDOW_HEIGHT};

// CENTER
constexpr unsigned int WINDOW_CENTER_X = WINDOW_WIDTH / 2.f;
constexpr unsigned int WINDOW_CENTER_Y = WINDOW_HEIGHT / 2.f;
constexpr sf::Vector2f WINDOW_CENTER = {WINDOW_CENTER_X, WINDOW_CENTER_Y};

// FPS
constexpr int FRAMES_PER_SECOND = 60;