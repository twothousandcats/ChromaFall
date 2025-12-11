#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

constexpr float LASER_WIDTH = 4.f;
constexpr sf::Color LASER_COLOR = sf::Color::Yellow;
constexpr float LASER_POS_X = 0.f;
constexpr float LASER_POS_Y = 0.f;
constexpr sf::Vector2f LASER_ORIGIN = {LASER_POS_X, LASER_POS_Y};

constexpr static float LASER_BASE_DMG = 0.05f;