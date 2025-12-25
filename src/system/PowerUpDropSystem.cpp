#include "systems/PowerUpDropSystem.hpp"

#include <vector>

#include "components/Position.hpp"
#include "components/PowerUpPickup.hpp"
#include "components/Shape.hpp"
#include "components/TextureAnimation.hpp"
#include "components/Textured.hpp"
#include "components/Velocity.hpp"
#include "config/SetupConfig.hpp"
#include "data/PowerUpType.hpp"
#include "utils/PowerUpUtils.hpp"

void PowerUpDropSystem::textureFallback(
    Entity *pickup
) {
    auto *shape = pickup->getComponent<Shape>();
    auto *anim = pickup->getComponent<TextureAnimation>();
    const auto *textured = pickup->getComponent<Textured>();

    if (shape && anim && textured && textured->value) {
        // начальный кадр
        shape->shape.setTextureRect({
            {0, 0},
            {
                anim->frameWidth,
                anim->frameHeight
            }
        });
        // сброс цвета текстуры
        shape->shape.setFillColor(sf::Color::White);
    }
}

PowerUpDropSystem::PowerUpDropSystem() {
    if (!powerUpTexture.loadFromFile(POWER_UP_DROP_TEXTURE_PATH)) {
        exit(1);
    }
}

void PowerUpDropSystem::tryDropPowerUp(
    std::vector<std::unique_ptr<Entity> > &pickups,
    const sf::Vector2f &spawnPos,
    WeaponType currentWeapon,
    std::mt19937 &rng
) {
    // сген > POWER_UP_DROP_CHANCE_PERCENT
    static std::uniform_real_distribution<float> dist(0.0f, 100.0f);
    if (dist(rng) > POWER_UP_DROP_CHANCE_PERCENT) {
        return;
    }

    // усиления
    auto candidates = getAvailablePowerUps(currentWeapon);
    if (candidates.empty()) return;

    std::uniform_int_distribution<size_t> typeDist(0, candidates.size() - 1);
    PowerUpType chosen = candidates[typeDist(rng)];

    // Создаём сущность
    auto pickup = std::make_unique<Entity>();
    pickup->addComponent(std::make_unique<Position>(spawnPos.x, spawnPos.y));
    pickup->addComponent(std::make_unique<PowerUpPickup>(chosen));
    pickup->addComponent(std::make_unique<Velocity>(0.f, POWER_UP_DROP_SPEED));

    const sf::Texture *texture = &powerUpTexture;
    if (texture) {
        pickup->addComponent(std::make_unique<Shape>(POWER_UP_PICKUP_RADIUS, POWER_UP_PICKUP_RADIUS, sf::Color::White));
        pickup->addComponent(std::make_unique<Textured>(*texture));
        pickup->addComponent(std::make_unique<TextureAnimation>(
            POWER_UP_DROP_FRAMES_COUNT,
            POWER_UP_DROP_FRAME_SIDE,
            POWER_UP_DROP_FRAME_SIDE,
            POWER_UP_DROP_FRAME_DURATION
        ));
    } else {
        pickup->addComponent(std::make_unique<Shape>(POWER_UP_PICKUP_RADIUS, POWER_UP_PICKUP_RADIUS, sf::Color::White));
    }
    textureFallback(pickup.get());
    pickups.push_back(std::move(pickup));
}
