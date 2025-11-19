#pragma once
#include "../core/Component.hpp"
#include <SFML/Graphics.hpp>

struct Renderable final : Component {
    sf::RectangleShape shape;

    explicit Renderable(
        float w = 20,
        float h = 20,
        const sf::Color color = sf::Color::Cyan
    ) {
        shape.setSize({w, h});
        shape.setOrigin({w / 2.f, h / 2.f});
        shape.setFillColor(color);
    }
};
