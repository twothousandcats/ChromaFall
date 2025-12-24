#pragma once
#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>

#include "components/Upgrades.hpp"
#include "config/BulletConfig.hpp"

enum class WeaponType;
class Entity;

class ShootSystem {
public:
    void update(
        std::vector<std::unique_ptr<Entity> > &bullets,
        const sf::Vector2f &muzzlePos,
        int bulletsCount = DEFAULT_BULLETS_COUNT,
        float shootingCooldown = SHOOTING_COOLDOWN,
        float damage = BULLET_BASE_DMG,
        float spreadAngle = DEFAULT_BULLETS_SPREAD_FACTOR, // shotgun
        WeaponType weaponType = WeaponType::BLASTER
    );

private:
    sf::Clock shootClock;
};
