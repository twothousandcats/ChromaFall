#include "systems/ShootSystem.hpp"

#include <iostream>

#include "components/Bullet.hpp"
#include "components/Position.hpp"
#include "components/Renderable.hpp"
#include "components/Velocity.hpp"
#include "entities/Entity.hpp"

constexpr float BULLET_WIDTH = 4.f;
constexpr float BULLET_HEIGHT = 20.f;
constexpr sf::Vector2f BULLET_SIZE = {BULLET_WIDTH, BULLET_HEIGHT};
constexpr sf::Color BULLET_COLOR = {0, 204, 109, 255};

constexpr sf::Vector2f BULLET_VELOCITY_TRACE = {0.f, 200.f};

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
    float baseX = playerPosition.x + 10.f - BULLET_WIDTH / 2.f;
    float baseY = playerPosition.y - BULLET_HEIGHT; // TODO: заменить, когда будет вводиться файл констант

    for (int i = 0; i < bulletsCount; ++i) {
        // bullet creation
        auto bullet = std::make_unique<Entity>();

        float offsetX = 0.f;
        if (bulletsCount > 1) {
            offsetX = (i - (bulletsCount - 1) / 2.f) * 15.f;
        }

        float bulletX = baseX + offsetX;
        float bulletY = baseY;
        // определяем компонент позиции для пули
        bullet->addComponent(std::make_unique<Position>(bulletX, bulletY));
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
