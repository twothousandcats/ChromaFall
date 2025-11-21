#include "data/AsteroidTemplate.hpp"
#include <vector>

constexpr float HP_FACTOR = 0.4;
constexpr float LARGE_ASTEROID_HP = 3.f;
constexpr float MEDIUM_ASTEROID_HP = LARGE_ASTEROID_HP * HP_FACTOR;
constexpr float SMALL_ASTEROID_HP = MEDIUM_ASTEROID_HP * HP_FACTOR;

constexpr float RADIUS_FACTOR = 0.5f;
constexpr float LARGE_ASTEROID_RADIUS = 60.f;
constexpr float MEDIUM_ASTEROID_RADIUS = LARGE_ASTEROID_RADIUS * RADIUS_FACTOR;
constexpr float SMALL_ASTEROID_RADIUS = MEDIUM_ASTEROID_RADIUS * RADIUS_FACTOR;

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
