#include <vector>

#include "components/Upgrades.hpp"
#include "data/PowerUpType.hpp"

std::vector<PowerUpType> getAvailablePowerUps(WeaponType currentWeapon) {
    std::vector<PowerUpType> options = {
        PowerUpType::EXTRA_HP,
        PowerUpType::EXTRA_DAMAGE,
        PowerUpType::EXTRA_BULLET,
        PowerUpType::SHOOTING_COOLDOWN
    };

    if (currentWeapon != WeaponType::SHOTGUN) {
        options.push_back(PowerUpType::WEAPON_SHOTGUN);
    }
    if (currentWeapon != WeaponType::LASER) {
        options.push_back(PowerUpType::WEAPON_LASER);
    }
    if (currentWeapon != WeaponType::BLASTER) {
        options.push_back(PowerUpType::WEAPON_BLASTER);
    }

    return options;
}
