#pragma once

#include "core/Component.hpp"

struct Damage final : Component {
    float value = 0.f;

    explicit Damage(float v) : value(v) {}
};