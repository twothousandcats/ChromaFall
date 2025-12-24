#pragma once
#include "../core/Component.hpp"
#include <SFML/Graphics.hpp>

struct Shape final : Component {
    sf::RectangleShape shape;

    explicit Shape(
        float w = 20.f,
        float h = 20.f,
        const sf::Color color = sf::Color::Cyan
    ) {
        shape.setSize({w, h});
        shape.setOrigin({w / 2.f, h / 2.f});
        shape.setFillColor(color);
        // TODO: убрать перед релизом
        // shape.setOutlineColor(color);
        // shape.setOutlineThickness(1);
    }
};
