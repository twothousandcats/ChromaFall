#include "data/AsteroidTemplate.hpp"
#include <vector>

#include "config/AsteroidConfig.hpp"


const std::vector<AsteroidTemplate> &getAsteroidTemplates() {
    static std::vector<AsteroidTemplate> templates = {
        {
            AsteroidSize::LARGE,
            LARGE_ASTEROID_RADIUS,
            LARGE_ASTEROID_HP,
            sf::Color::Cyan,
        },
        {
            AsteroidSize::MEDIUM,
            MEDIUM_ASTEROID_RADIUS,
            MEDIUM_ASTEROID_HP,
            sf::Color::Magenta,
        },
        {
            AsteroidSize::SMALL,
            SMALL_ASTEROID_RADIUS,
            SMALL_ASTEROID_HP,
            sf::Color::Yellow,
        },
    };

    return templates;
};
