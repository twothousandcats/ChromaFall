#pragma once

#include "core/Component.hpp"

struct TextureAnimation final : Component {
    float frameDuration = 0.1f;
    float currentTime = 0.f;
    int currentFrame = 0;
    int frameCount = 1;
    int frameWidth = 16;
    int frameHeight = 16;
    bool playing = true;

    TextureAnimation(
        int count,
        int width,
        int height,
        float duration
    ) : frameDuration(duration), frameCount(count), frameWidth(width), frameHeight(height) {
    }
};
