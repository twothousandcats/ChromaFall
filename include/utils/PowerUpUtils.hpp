#pragma once
#include "data/PowerUpType.hpp"
#include <vector>
#include "components/Upgrades.hpp"

std::vector<PowerUpType> getAvailablePowerUps(WeaponType currentWeapon);
