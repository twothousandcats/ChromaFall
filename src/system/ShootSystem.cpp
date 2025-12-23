#include "systems/ShootSystem.hpp"

#include "components/Bullet.hpp"
#include "components/Damage.hpp"
#include "components/Position.hpp"
#include "components/Shape.hpp"
#include "components/Velocity.hpp"

#include "entities/Entity.hpp"

#include "config/BulletConfig.hpp"
#include "config/PlayerConfig.hpp"
#include "config/SetupConfig.hpp"

void ShootSystem::update(
    std::vector<std::unique_ptr<Entity> > &bullets,
    const sf::RenderWindow &window,
    const sf::Vector2f &playerPosition,
    int bulletsCount,
    float shootingCooldown,
    float damage,
    float spreadAngle
) {
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        return;
    }

    if (shootClock.getElapsedTime().asSeconds() < shootingCooldown) {
        return;
    }

    float muzzleX = playerPosition.x + PLAYER_SIDE / HALF_DIVISOR - BULLET_WIDTH / HALF_DIVISOR;
    float muzzleY = playerPosition.y;

    for (int i = 0; i < bulletsCount; ++i) {
        auto bullet = std::make_unique<Entity>();
        bullet->addComponent(std::make_unique<Damage>(damage));

        float angleRad = -M_PI_2;

        if (bulletsCount > 1) {
            // spread/2
            float halfSpreadRad = sf::degrees(spreadAngle / HALF_DIVISOR).asRadians();
            float angleStep = (HALF_DIVISOR * halfSpreadRad) / (bulletsCount - 1);
            float currentAngle = -halfSpreadRad + i * angleStep;
            angleRad += currentAngle;
        }

        bullet->addComponent(std::make_unique<Position>(muzzleX, muzzleY - BULLET_HEIGHT));

        sf::Vector2f velocity(
            std::cos(angleRad) * BULLET_SPEED,
            std::sin(angleRad) * BULLET_SPEED
        );
        bullet->addComponent(std::make_unique<Velocity>(velocity.x, velocity.y));

        // Рендер
        auto renderable = std::make_unique<Shape>();
        renderable->shape.setSize(BULLET_SIZE);
        renderable->shape.setFillColor(BULLET_COLOR);
        bullet->addComponent(std::move(renderable));

        bullet->addComponent(std::make_unique<Bullet>());
        bullets.push_back(std::move(bullet));
    }

    shootClock.restart();
}
