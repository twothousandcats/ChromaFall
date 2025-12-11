#pragma once

#include "config/UpgradesConfig.hpp"
#include "core/Component.hpp"

enum class WeaponType {
    BLASTER,
    SHOTGUN,
    LASER
};

struct Upgrades final : Component {
    float extraMaxHp = UPGRADES_BASE_HP;
    float damageMultiplier = UPGRADES_BASE_DMG_MULT;
    int extraBulletCount = UPGRADES_BASE_BULLET_COUNT;
    float shootingCooldown = UPGRADES_BASE_SHOOTING_COOLDOWN_MULT;

    WeaponType weapon = WeaponType::BLASTER;
};
