#pragma once

#include "config/BulletConfig.hpp"

constexpr float UPGRADES_BASE_HP = 1.f;
constexpr float UPGRADES_BASE_DMG_MULT = 1.f;
constexpr int UPGRADES_BASE_BULLET_COUNT = 0;
constexpr float UPGRADES_BASE_SHOOTING_COOLDOWN_MULT = 1.f;

// powerups
constexpr float POWERUP_DMG_MULT = 1.2f;
constexpr float POWERUP_SHOOTING_COOLDOWN_MULT = 0.9f;
constexpr int POWERUP_BULLET_ADD = 1;

// DROP POWERUP
constexpr float POWER_UP_DROP_CHANCE_PERCENT = 5.0f; // 5%
constexpr float POWER_UP_PICKUP_RADIUS = 40.f; // size
constexpr float POWER_UP_DROP_SPEED = 100.f; // speed
constexpr auto POWER_UP_DROP_TEXTURE_PATH = "assets/images/textures/power_item.png";
constexpr int POWER_UP_DROP_FRAMES_COUNT = 3;
constexpr int POWER_UP_DROP_FRAME_SIDE = 16;
constexpr float POWER_UP_DROP_FRAME_DURATION = 0.3f;
