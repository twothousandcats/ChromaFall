#pragma  once
#include <SFML/Graphics/Color.hpp>

#include "AsteroidConfig.hpp"

constexpr sf::Color BOSS_COLOR_DEFAULT = {131, 208, 65, 255};
constexpr sf::Color BOSS_COLOR_TYPE_MEDIUM = {255, 202, 54, 255};
constexpr sf::Color BOSS_COLOR_TYPE_HARD = {236, 105, 181, 255};

constexpr float DOUBLE_FACTOR = 2.f;
constexpr float BOSS_ATTACK_INTERVAL = 3.f;
constexpr float BOSS_LASER_DURATION = 3.f;
constexpr float BOSS_LASER_WARNING_TIME = 1.f;
constexpr float BOSS_LASER_ACTIVE = 1.f;

// EASY
constexpr float EASY_BOSS_HP = 5.f;
constexpr float BOSS_SIZE = 60.f;

// MEDIUM
constexpr float MEDIUM_BOSS_HP = EASY_BOSS_HP * DOUBLE_FACTOR;

// HARD
constexpr float HARD_BOSS_HP = MEDIUM_BOSS_HP * DOUBLE_FACTOR;

// behavior
constexpr float BOSS_AMPLITUDE = 180.f; // амплитуда синусоиды
constexpr float BOSS_FREQUENCY = 2.f; // частота синусоиды

constexpr int BOSS_ASTEROIDS_TO_BLOW_COUNT = 25;
constexpr sf::Color BOSS_ASTEROIDS_TO_BLOW_COLOR = {255, 0, 0, 255};
constexpr float BOSS_ASTEROIDS_TO_BLOW_SPEED = 120.f;
constexpr float BOSS_ASTEROIDS_TO_BLOW_OFFSET_FACTOR = 0.25f;
constexpr float BOSS_ASTEROIDS_TO_BLOW_ACCELERATION = 50.f;
constexpr auto BOSS_ASTEROIDS_TO_BLOW_TEXTURE_PATH = ASTEROID_TEXTURE_PATH_MEDIUM;

// entry phase
constexpr float BOSS_ENTRY_Y_POS = 150.f;
constexpr float BOSS_ENTRY_Y_SPEED = 120.f;

// zone settings
constexpr float BOSS_ZONE_WIDTH = 200.f;
constexpr float BOSS_ZONE_HEIGHT = 100.f;

constexpr float BOSS_CHANGE_INTERVAL = 2.f;
constexpr float BOSS_STIFFNESS_FACTOR = 8.0f; // коэффициент ускорения в сторону цели
constexpr float BOSS_DAMPING = 4.0f; // коэффициент затухания скорости
constexpr float BOSS_ARRIVAL_DIST = 15.0f; // Радиус покоя вокруг цели
constexpr float BOSS_ARRIVAL_SPEED = 25.0f; //Максимальная скорость, при которой цель всё ещё считается достигнутой

constexpr auto BOSS_EASY_TEXTURE_PATH = "assets/images/textures/boss_easy.png";
constexpr auto BOSS_MEDIUM_TEXTURE_PATH = "assets/images/textures/boss_medium.png";
constexpr auto BOSS_HARD_TEXTURE_PATH = "assets/images/textures/boss_hard.png";
constexpr int BOSS_TEXTURE_FRAMES_COUNT = 5;
constexpr int BOSS_TEXTURE_FRAMES_COUNT_MEDIUM = 4;
constexpr int BOSS_TEXTURE_FRAME_SIDE = 16;
constexpr float BOSS_TEXTURE_FRAME_DURATION = 0.3f;