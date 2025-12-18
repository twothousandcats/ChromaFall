#pragma once

#include "core/Component.hpp"
#include "data/PowerUpType.hpp"

struct PowerUpPickup final : Component {
    PowerUpType type;

    explicit PowerUpPickup(PowerUpType t) : type(t) {
    }
};
