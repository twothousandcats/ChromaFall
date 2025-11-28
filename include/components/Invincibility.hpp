#pragma once


#include "core/Component.hpp"

struct Invincibility final : Component {
    float duration = 2.f;
    float timer = 0.f;

    explicit Invincibility(float d = 2.f) : duration(d), timer(0.f) {
    };

    void activate() {
        timer = duration;
    };

    void update(const float dt) {
        if (timer > 0.f) {
            timer -= dt;
            if (timer <= 0.f) {
                timer = 0.f;
            }
        }
    }

    bool isActive() const {
        return timer > 0.f;
    }
};
