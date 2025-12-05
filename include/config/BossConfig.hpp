#pragma  once
#include <SFML/Graphics/Color.hpp>

constexpr sf::Color BOSS_COLOR_DEFAULT = {136, 123, 232, 255};
constexpr sf::Color BOSS_COLOR_TYPE_MEDIUM = {136, 0, 232, 255};
constexpr sf::Color BOSS_COLOR_TYPE_HARD = {242, 192, 220, 255};

constexpr float DOUBLE_FACTOR = 2.f;
constexpr float BOSS_ATTACK_INTERVAL = 3.f;
constexpr float BOSS_LASER_DURATION = 3.f;
constexpr float BOSS_LASER_ACTIVE = 1.f;

// EASY
constexpr float EASY_BOSS_HP = 5.f;
constexpr float BOSS_SIZE = 60.f;

// MEDIUM
constexpr float MEDIUM_BOSS_HP = EASY_BOSS_HP * DOUBLE_FACTOR;

// HARD
constexpr float HARD_BOSS_HP = MEDIUM_BOSS_HP * DOUBLE_FACTOR;

constexpr float BOSS_AMPLITUDE = 180.f; // амплитуда синусоиды
constexpr float BOSS_FREQUENCY = 2.f; // частота синусоиды

constexpr int BOSS_ASTEROIDS_TO_BLOW_COUNT = 25;
constexpr sf::Color BOSS_ASTEROIDS_TO_BLOW_COLOR = {255, 0, 0, 255};
constexpr float BOSS_ASTEROIDS_TO_BLOW_SPEED = 120.f;
constexpr float BOSS_ASTEROIDS_TO_BLOW_OFFSET_FACTOR = 0.25f;
constexpr float BOSS_ASTEROIDS_TO_BLOW_ACCELERATION = 50.f;

// entry phase
constexpr float BOSS_ENTRY_Y_POS = 100.f;
constexpr float BOSS_ENTRY_Y_SPEED = 120.f;