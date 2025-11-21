#pragma once

#include  <SFML/Graphics.hpp>

// SIZE
constexpr float BULLET_WIDTH = 4.f;
constexpr float BULLET_HEIGHT = 20.f;
constexpr sf::Vector2f BULLET_SIZE = {BULLET_WIDTH, BULLET_HEIGHT};

// SHAPE
constexpr sf::Color BULLET_COLOR = {0, 204, 109, 255};

// VELOCITY
constexpr sf::Vector2f BULLET_VELOCITY_TRACE = {0.f, 200.f};
