#pragma once
#include "../core/Component.hpp"
#include <SFML/Graphics.hpp>

struct Renderable : public Component {
    sf::RectangleShape shape;

    Renderable(
        float w = 20,
        float h = 20
    ) {
        shape.setSize({w, h});
        shape.setOrigin({w / 2, h / 2});
        shape.setFillColor(sf::Color::Cyan);
    }
};
