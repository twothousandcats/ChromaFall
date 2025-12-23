#include "systems/BossSystem.hpp"

#include "components/Boss.hpp"
#include "components/Health.hpp"
#include "components/Position.hpp"
#include "components/Velocity.hpp"
#include "components/Shape.hpp"
#include "components/BossPatternData.hpp"

#include "config/BossConfig.hpp" // где будут HP и размеры
#include "config/SetupConfig.hpp"

std::unique_ptr<Entity> BossSystem::spawnBoss(BossType type) {
    // init
    int maxHp = 0;
    float size = 0.f;
    sf::Color color;

    switch (type) {
        case BossType::EASY:
            maxHp = EASY_BOSS_HP;
            size = BOSS_SIZE;
            color = BOSS_COLOR_DEFAULT;
            break;
        case BossType::MEDIUM:
            maxHp = MEDIUM_BOSS_HP;
            size = BOSS_SIZE;
            color = BOSS_COLOR_TYPE_MEDIUM;
            break;
        case BossType::HARD:
            maxHp = HARD_BOSS_HP;
            size = BOSS_SIZE;
            color = BOSS_COLOR_TYPE_HARD;
            break;
    }

    auto boss = std::make_unique<Entity>();
    boss->addComponent(std::make_unique<Boss>(type, maxHp));
    boss->addComponent(std::make_unique<Health>(static_cast<float>(maxHp)));
    boss->addComponent(std::make_unique<Position>(WINDOW_CENTER_X, 0.f - BOSS_SIZE));
    boss->addComponent(std::make_unique<Velocity>());
    boss->addComponent(std::make_unique<Shape>(size, size, color));
    boss->addComponent(std::make_unique<BossPatternData>());

    return boss;
}
