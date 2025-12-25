#pragma once
#include <SFML/Graphics/Color.hpp>

#include "SetupConfig.hpp"

constexpr int TRAP_LASER_COUNT_MIN = 2;
constexpr int TRAP_LASER_COUNT_MAX = 5;

constexpr float TRAP_LASER_MARGIN = 50.f;

constexpr float TRAP_LASER_ANGLE = 90.f;

constexpr float TRAP_LASER_BASE_WIDTH = 8.f;
constexpr float TRAP_LASER_BASE_HEIGHT = WINDOW_WIDTH * 3.f;
constexpr float TRAP_LASER_BASE_X_ORIGIN = TRAP_LASER_BASE_WIDTH / 2.f;
constexpr float TRAP_LASER_BASE_Y_ORIGIN = TRAP_LASER_BASE_HEIGHT / 2.f;
constexpr sf::Color TRAP_LASER_BASE_COLOR = {255, 0, 0, 200};
constexpr sf::Color TRAP_LASER_WARNING_COLOR = {255, 0, 0, 100};