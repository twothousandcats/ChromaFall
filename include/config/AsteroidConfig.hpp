#pragma once

// HP
constexpr float HP_FACTOR = 0.4;
constexpr float LARGE_ASTEROID_HP = 3.f;
constexpr float MEDIUM_ASTEROID_HP = LARGE_ASTEROID_HP * HP_FACTOR;
constexpr float SMALL_ASTEROID_HP = MEDIUM_ASTEROID_HP * HP_FACTOR;

// SIZE
constexpr float RADIUS_FACTOR = 0.5f;
constexpr float LARGE_ASTEROID_RADIUS = 60.f;
constexpr float MEDIUM_ASTEROID_RADIUS = LARGE_ASTEROID_RADIUS * RADIUS_FACTOR;
constexpr float SMALL_ASTEROID_RADIUS = MEDIUM_ASTEROID_RADIUS * RADIUS_FACTOR;

// CREATION SETUP
constexpr float ASTEROID_EDGE_X = 0;
constexpr float ASTEROID_EDGE_VELOCITY = 80.f;
constexpr float ASTEROID_LINEAR_ACCELERATION = 50.f;
constexpr float BASE_SPAWN_INTERVAL = 1.f;
constexpr float INTERVAL_REDUCTION_FACTOR = 0.2f;

// DROP ANGLE
constexpr float ASTEROID_ANGLE_SPREAD_FACTOR = 0.5f;

constexpr auto ASTEROID_TEXTURE_PATH_SMALL = "assets/images/textures/asteroid_s.png";
constexpr auto ASTEROID_TEXTURE_PATH_MEDIUM = "assets/images/textures/asteroid_m.png";
constexpr auto ASTEROID_TEXTURE_PATH_LARGE = "assets/images/textures/asteroid_l.png";