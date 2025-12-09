#pragma once

#include "config/LevelConfig.hpp"
#include "core/Component.hpp"

struct Experience final : Component {
    int value = LEVEL_START_EXP;
    float neededValue = LEVEL_BASE_NEEDED_COUNT_EXP;

    explicit Experience(
        const int v = LEVEL_START_EXP,
        const float nV = LEVEL_BASE_NEEDED_COUNT_EXP
        ) : value(v), neededValue(nV) {
    };
};
