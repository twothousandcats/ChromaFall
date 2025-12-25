#pragma once
#include "config/PlayerConfig.hpp"
#include "core/Component.hpp"

struct Health final : Component {
    float value = PLAYER_BASE_HP;

    explicit Health(float hp = 1.f) : value(hp) {}
};
