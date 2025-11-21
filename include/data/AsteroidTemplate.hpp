#pragma once
#include <vector>
#include <SFML/Graphics/Color.hpp>

#include "components/Asteroid.hpp"


struct AsteroidTemplate {
    AsteroidSize size;
    float radius;
    float health;
    sf::Color color;
};

const std::vector<AsteroidTemplate> &getAsteroidTemplates();
