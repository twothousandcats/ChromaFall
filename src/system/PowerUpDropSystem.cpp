#include "systems/PowerUpDropSystem.hpp"

#include <vector>

#include "components/Position.hpp"
#include "components/PowerUpPickup.hpp"
#include "components/Shape.hpp"
#include "components/Velocity.hpp"
#include "data/PowerUpType.hpp"
#include "utils/PowerUpUtils.hpp"

void PowerUpDropSystem::tryDropPowerUp(
    std::vector<std::unique_ptr<Entity> > &pickups,
    const sf::Vector2f &spawnPos,
    WeaponType currentWeapon,
    std::mt19937 &rng
) {
    static std::uniform_real_distribution<float> dist(0.0f, 100.0f);
    if (dist(rng) > POWER_UP_DROP_CHANCE_PERCENT) {
        return; // шанс
    }

    // усиления
    auto candidates = getAvailablePowerUps(currentWeapon);
    if (candidates.empty()) return;

    std::uniform_int_distribution<size_t> typeDist(0, candidates.size() - 1);
    PowerUpType chosen = candidates[typeDist(rng)];

    // Создаём сущность
    auto pickup = std::make_unique<Entity>();
    pickup->addComponent(std::make_unique<Position>(spawnPos.x, spawnPos.y));
    pickup->addComponent(std::make_unique<PowerUpPickup>(chosen));

    // TODO: перевод на текстуры
    auto renderable = std::make_unique<Shape>(
        POWER_UP_PICKUP_RADIUS * 2,
        POWER_UP_PICKUP_RADIUS * 2,
        sf::Color::White
    );
    renderable->shape.setOrigin({POWER_UP_PICKUP_RADIUS, POWER_UP_PICKUP_RADIUS});
    pickup->addComponent(std::move(renderable));
    pickup->addComponent(std::make_unique<Velocity>(0.f, POWER_UP_DROP_SPEED));

    pickups.push_back(std::move(pickup));
}
