#pragma once
#include "core/Component.hpp"

struct Rotation final : Component {
    float angle = 0.f;

    explicit Rotation(float curAngle = 0.f) : angle(curAngle) {}
};
