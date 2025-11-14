#pragma once
#include "../core/Component.hpp"
#include <SFML/System/Vector2.hpp>

class Velocity final : public Component {
    sf::Vector2f value{0.f, 0.f};
};
