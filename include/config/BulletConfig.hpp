#pragma once
#include "SFML/Graphics.hpp"

// settings
constexpr int DEFAULT_BULLETS_COUNT = 1;
constexpr int SHOTGUN_BULLETS_COUNT = 3;
constexpr float DEFAULT_BULLETS_SPREAD_FACTOR = 0.f;
constexpr float SHOTGUN_BULLETS_SPREAD_FACTOR = 30.f;
constexpr static float SHOOTING_COOLDOWN = 0.5f;
constexpr static float BULLET_BASE_DMG = 1.f;

// SIZE
constexpr float BULLET_WIDTH = 4.f;
constexpr float BULLET_HEIGHT = 20.f;
constexpr sf::Vector2f BULLET_SIZE = {BULLET_WIDTH, BULLET_HEIGHT};

// SHAPE
constexpr sf::Color BULLET_COLOR = {0, 204, 109, 255};

// VELOCITY
constexpr float BULLET_SPEED = 200.f;
