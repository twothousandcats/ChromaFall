#pragma once
#include "core/Component.hpp"

struct Health final : Component {
    float value = 3.f;
    explicit Health(float hp = 1.f) : value(hp) {}
};
