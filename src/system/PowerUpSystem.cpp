#include "systems/PowerUpSystem.hpp"
#include <random>
#include "algorithm"
#include "entities/Entity.hpp"
#include "components/Upgrades.hpp"
#include "components/Health.hpp"

std::vector<PowerUpType> PowerUpSystem::generateOptions(
    int count
) {
    std::vector allOptions = {
        PowerUpType::EXTRA_HP,
        PowerUpType::EXTRA_DAMAGE,
        PowerUpType::EXTRA_BULLET,
        PowerUpType::WEAPON_SHOTGUN,
        PowerUpType::WEAPON_LASER
    };

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(allOptions.begin(), allOptions.end(), g);

    if (count > static_cast<int>(allOptions.size())) {
        count = allOptions.size();
    }
    return {allOptions.begin(), allOptions.begin() + count};
}

void PowerUpSystem::apply(
    Entity *player,
    PowerUpType type
) {
    if (!player) {
        return;
    }

    auto *upgrades = player->getComponent<Upgrades>();
    auto *health = player->getComponent<Health>();
    if (!upgrades || !health) {
        return;
    }

    switch (type) {
        case PowerUpType::EXTRA_HP:
            health->value += upgrades->extraMaxHp;
            break;
        case PowerUpType::EXTRA_DAMAGE:
            upgrades->damageMultiplier += 0.2f;
            break;
        case PowerUpType::EXTRA_BULLET:
            upgrades->extraBulletCount += 1;
            break;
        case PowerUpType::WEAPON_SHOTGUN:
            upgrades->weapon = WeaponType::Shotgun;
            break;
        case PowerUpType::WEAPON_LASER:
            upgrades->weapon = WeaponType::Laser;
            break;
    }
}
