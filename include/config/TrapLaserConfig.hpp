#pragma once
#include <SFML/Graphics/Color.hpp>

#include "SetupConfig.hpp"

constexpr int TRAP_LASER_COUNT_MIN = 3;
constexpr int TRAP_LASER_COUNT_MAX = 5;

constexpr float TRAP_LASER_MARGIN = 50.f;

constexpr float TRAP_LASER_BASE_WIDTH = 8.f;
constexpr float TRAP_LASER_BASE_HEIGHT = WINDOW_HEIGHT;
constexpr sf::Color TRAP_LASER_BASE_COLOR = {255, 0, 0, 200};