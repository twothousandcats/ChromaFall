#include "systems/PowerUpSystem.hpp"
#include <random>
#include "algorithm"
#include "entities/Entity.hpp"
#include "components/Upgrades.hpp"
#include "components/Health.hpp"

std::vector<PowerUpType> PowerUpSystem::generateOptions(
    int count,
    WeaponType currentWeapon
) {
    std::vector allOptions = {
        PowerUpType::EXTRA_HP,
        PowerUpType::EXTRA_DAMAGE,
        PowerUpType::EXTRA_BULLET,
        PowerUpType::SHOOTING_COOLDOWN,
    };

    if (currentWeapon != WeaponType::SHOTGUN) {
        allOptions.push_back(PowerUpType::WEAPON_SHOTGUN);
    }
    if (currentWeapon != WeaponType::LASER) {
        allOptions.push_back(PowerUpType::WEAPON_LASER);
    }
    if (currentWeapon != WeaponType::BLASTER) { // если используешь
        allOptions.push_back(PowerUpType::WEAPON_BLASTER);
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(allOptions.begin(), allOptions.end(), g);

    if (count > static_cast<int>(allOptions.size())) {
        count = static_cast<int>(allOptions.size());
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
            upgrades->damageMultiplier *= POWERUP_DMG_MULT;
            break;
        case PowerUpType::EXTRA_BULLET:
            upgrades->extraBulletCount += POWERUP_BULLET_ADD;
            break;
        case PowerUpType::SHOOTING_COOLDOWN:
            upgrades->shootingCooldown *= POWERUP_SHOOTING_COOLDOWN_MULT;
            break;
        case PowerUpType::WEAPON_SHOTGUN:
            upgrades->weapon = WeaponType::SHOTGUN;
            break;
        case PowerUpType::WEAPON_LASER:
            upgrades->weapon = WeaponType::LASER;
            break;
        case PowerUpType::WEAPON_BLASTER:
            upgrades->weapon = WeaponType::BLASTER;
            break;
    }
}
