#pragma once

#include "config/BossConfig.hpp"
#include "core/Component.hpp"

enum class BossType {
    EASY,
    MEDIUM,
    HARD
};

struct Boss final : Component {
    BossType type;
    int maxHp; // для UI

    explicit Boss(BossType t, int hp) : type(t), maxHp(hp) {}
};
