#pragma once

#include "core/Component.hpp"

struct BossPatternData final : Component {
    float timeSinceSpawn = 0.f;
    float lastAttackTime = 0.f;
    bool hasEntered = false;
    float sineStartTime = 0.f;
};
