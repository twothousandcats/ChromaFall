#pragma once
#include <SFML/Graphics/Texture.hpp>

#include "core/Component.hpp"

struct Textured final : Component {
    const sf::Texture *value = nullptr;

    explicit Textured(const sf::Texture &tex) : value(&tex) {
    }

    // запрет копирования
    Textured(const Textured &) = delete;
    Textured &operator=(const Textured &) = delete;
};
