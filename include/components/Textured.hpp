#pragma once
#include <SFML/Graphics/Texture.hpp>

#include "core/Component.hpp"

struct Textured final : Component {
    const sf::Texture *value = nullptr;

    explicit Textured(const sf::Texture &tex) : value(&tex) {
    }
};
