#include "systems/BossSystem.hpp"

#include "components/Boss.hpp"
#include "components/Health.hpp"
#include "components/Position.hpp"
#include "components/Velocity.hpp"
#include "components/Renderable.hpp"
#include "components/BossPatternData.hpp"

#include "config/BossConfig.hpp" // где будут HP и размеры
#include "config/SetupConfig.hpp"

std::unique_ptr<Entity> BossSystem::spawnBoss(BossType type) {
    int maxHp = 0;
    float size = 0.f;
    sf::Color color;

    switch (type) {
        case BossType::EASY:
            maxHp = EASY_BOSS_HP;
            size = EASY_BOSS_SIZE;
            color = BOSS_COLOR;
            break;
        case BossType::MEDIUM:
            maxHp = MEDIUM_BOSS_HP;
            size = MEDIUM_BOSS_SIZE;
            color = BOSS_COLOR;
            break;
        case BossType::HARD:
            maxHp = HARD_BOSS_HP;
            size = HARD_BOSS_SIZE;
            color = BOSS_COLOR;
            break;
    }

    auto boss = std::make_unique<Entity>();

    boss->addComponent(std::make_unique<Boss>(type, maxHp));
    boss->addComponent(std::make_unique<Health>(static_cast<float>(maxHp)));
    boss->addComponent(std::make_unique<Position>(100.f, WINDOW_CENTER_Y));
    boss->addComponent(std::make_unique<Velocity>(0.f, 0.f));
    boss->addComponent(std::make_unique<Renderable>(size, size, color));
    boss->addComponent(std::make_unique<BossPatternData>());

    return boss;
}
