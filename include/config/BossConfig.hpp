#pragma  once
#include <SFML/Graphics/Color.hpp>

constexpr sf::Color BOSS_COLOR = {136, 123, 232, 255};
constexpr float DOUBLE_FACTOR = 2.f;
constexpr float BOSS_ATTACK_INTERVAL = 3.f;
constexpr float BOSS_LASER_DURATION = 3.f;
constexpr float BOSS_LASER_ACTIVE = 1.f;

// EASY
constexpr float EASY_BOSS_HP = 5.f;
constexpr float EASY_BOSS_SIZE = 60.f;

// MEDIUM
constexpr float MEDIUM_BOSS_HP = EASY_BOSS_HP * DOUBLE_FACTOR;
constexpr float MEDIUM_BOSS_SIZE = EASY_BOSS_SIZE;

// HARD
constexpr float HARD_BOSS_HP = MEDIUM_BOSS_HP * DOUBLE_FACTOR;
constexpr float HARD_BOSS_SIZE = EASY_BOSS_SIZE;