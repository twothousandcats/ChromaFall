#pragma once

#include "core/Component.hpp"

struct Level final : Component {
    int value = 0;

    explicit Level(int v = 1) : value(v) {}
};
