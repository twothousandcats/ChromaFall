#include "systems/ShootSystem.hpp"

#include "components/Bullet.hpp"
#include "components/Position.hpp"
#include "components/Renderable.hpp"
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
    float spreadAngle
) {
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        return;
    }

    if (shootClock.getElapsedTime().asSeconds() < SHOOTING_COOLDOWN) {
        return;
    }
    float baseX = playerPosition.x + PLAYER_SIDE / HALF_DIVISOR  - BULLET_WIDTH / HALF_DIVISOR;
    float baseY = playerPosition.y - BULLET_HEIGHT;

    for (int i = 0; i < bulletsCount; ++i) {
        // bullet creation
        auto bullet = std::make_unique<Entity>();

        float offsetX = 0.f;
        if (bulletsCount > 1) {
            offsetX = (i - (bulletsCount - 1) / HALF_DIVISOR) * 15.f; // TODO: при реализации разброса (конфиг пуль)
        }

        float bulletX = baseX + offsetX;
        float bulletY = baseY;
        bullet->addComponent(std::make_unique<Position>(bulletX, bulletY)); // определяем компонент позиции для пули
        // определяем компоненту постоянной скорости
        bullet->addComponent(std::make_unique<Velocity>(BULLET_VELOCITY_TRACE.x, -BULLET_VELOCITY_TRACE.y));

        // рендеринг пули
        auto renderable = std::make_unique<Renderable>();
        renderable->shape.setSize(BULLET_SIZE);
        renderable->shape.setFillColor(BULLET_COLOR);
        bullet->addComponent(std::move(renderable));

        // Маркер
        bullet->addComponent(std::make_unique<Bullet>());
        bullets.push_back(std::move(bullet));
    }

    shootClock.restart();
}
