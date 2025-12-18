#include "systems/PowerUpDropSystem.hpp"

#include <vector>

#include "components/Position.hpp"
#include "components/PowerUpPickup.hpp"
#include "components/Renderable.hpp"
#include "components/Velocity.hpp"
#include "data/PowerUpType.hpp"

void PowerUpDropSystem::tryDropPowerUp(
    std::vector<std::unique_ptr<Entity> > &pickups,
    const sf::Vector2f &spawnPos,
    WeaponType currentWeapon,
    std::mt19937 &rng
) {
    static std::uniform_real_distribution<float> dist(0.0f, 100.0f);
    if (dist(rng) > POWER_UP_DROP_CHANCE_PERCENT) {
        return; // 5% шанс
    }

    // Соберём все возможные усиления, кроме текущего оружия (если оно уже разблокировано)
    std::vector<PowerUpType> candidates = {
        PowerUpType::EXTRA_HP,
        PowerUpType::EXTRA_DAMAGE,
        PowerUpType::EXTRA_BULLET,
        PowerUpType::SHOOTING_COOLDOWN
    };

    // оружие
    if (currentWeapon != WeaponType::BLASTER) candidates.push_back(PowerUpType::WEAPON_BLASTER);
    if (currentWeapon != WeaponType::SHOTGUN) candidates.push_back(PowerUpType::WEAPON_SHOTGUN);
    if (currentWeapon != WeaponType::LASER) candidates.push_back(PowerUpType::WEAPON_LASER);

    if (candidates.empty()) return;

    std::uniform_int_distribution<size_t> typeDist(0, candidates.size() - 1);
    PowerUpType chosen = candidates[typeDist(rng)];

    // Создаём сущность
    auto pickup = std::make_unique<Entity>();
    pickup->addComponent(std::make_unique<Position>(spawnPos.x, spawnPos.y));
    pickup->addComponent(std::make_unique<PowerUpPickup>(chosen));

    // TODO: допилить UI
    sf::Color color = sf::Color::White;
    switch (chosen) {
        case PowerUpType::EXTRA_HP: color = sf::Color::Red;
            break;
        case PowerUpType::EXTRA_DAMAGE: color = sf::Color::Cyan;
            break;
        case PowerUpType::EXTRA_BULLET: color = sf::Color::Green;
            break;
        case PowerUpType::SHOOTING_COOLDOWN: color = sf::Color::Yellow;
            break;
        case PowerUpType::WEAPON_BLASTER:
        case PowerUpType::WEAPON_SHOTGUN:
        case PowerUpType::WEAPON_LASER: color = sf::Color::Magenta;
            break;
    }

    auto renderable = std::make_unique<Renderable>(
        POWER_UP_PICKUP_RADIUS * 2,
        POWER_UP_PICKUP_RADIUS * 2,
        color
    );
    renderable->shape.setOrigin({POWER_UP_PICKUP_RADIUS, POWER_UP_PICKUP_RADIUS});
    pickup->addComponent(std::move(renderable));
    pickup->addComponent(std::make_unique<Velocity>(0.f, POWER_UP_DROP_SPEED));

    pickups.push_back(std::move(pickup));
}
