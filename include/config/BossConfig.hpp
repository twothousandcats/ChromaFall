#pragma  once
#include <SFML/Graphics/Color.hpp>

constexpr float BOSS_ATTACK_INTERVAL = 2.f;
constexpr float BOSS_BASE_HP = 10.f;
constexpr float BOSS_BASE_SIDE_SIZE = 80.f;
constexpr sf::Color BOSS_COLOR = {136, 123, 232, 255};

constexpr float BOSS_POSITION_SHIFT = 100.f;

constexpr int ASTEROIDS_TO_SHOOT_COUNT = 10;
constexpr float ASTEROIDS_ANGLE_SPEED = 0.4f;  // радианы
constexpr float ASTEROIDS_BASE_SPEED = 150.f;  // радианы

constexpr int LASER_COUNT = 3;
constexpr float BASE_X_DIST = 100.f;