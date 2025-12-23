#include "systems/LaserSystem.hpp"

#include "components/Damage.hpp"
#include "components/LaserBeam.hpp"
#include "components/Position.hpp"
#include "components/Shape.hpp"
#include "config/LaserBeamConfig.hpp"
#include "entities/Entity.hpp"

void LaserSystem::update(
    std::unique_ptr<Entity> &laserEntity,
    const sf::RenderWindow &window,
    const sf::Vector2f &playerMuzzlePosition,
    bool isShooting,
    bool isLaserWeapon,
    float damage,
    float length,
    float width
) {
    if (!isLaserWeapon || !isShooting) {
        laserEntity.reset();
        return;
    }

    if (!laserEntity) {
        laserEntity = std::make_unique<Entity>();
        laserEntity->addComponent(std::make_unique<Position>());
        laserEntity->addComponent(std::make_unique<Damage>(damage));
        laserEntity->addComponent(std::make_unique<LaserBeam>());

        auto renderable = std::make_unique<Shape>();
        renderable->shape.setFillColor(LASER_COLOR);
        renderable->shape.setOrigin(LASER_ORIGIN);
        laserEntity->addComponent(std::move(renderable));
    }

    // Обновляем
    auto* pos = laserEntity->getComponent<Position>();
    auto* damageComp = laserEntity->getComponent<Damage>();
    auto* render = laserEntity->getComponent<Shape>();

    if (pos && damageComp && render) {
        damageComp->value = damage;

        pos->value = playerMuzzlePosition - sf::Vector2f(0.f, length);

        render->shape.setSize({width, length});
        render->shape.setPosition(pos->value);
    }
}
