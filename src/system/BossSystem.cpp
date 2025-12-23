#include "systems/BossSystem.hpp"

#include "components/Boss.hpp"
#include "components/Health.hpp"
#include "components/Position.hpp"
#include "components/Velocity.hpp"
#include "components/Shape.hpp"
#include "components/BossPatternData.hpp"
#include "components/TextureAnimation.hpp"
#include "components/Textured.hpp"

#include "config/BossConfig.hpp" // где будут HP и размеры
#include "config/SetupConfig.hpp"

BossSystem::BossSystem() {
    if (!easyBossTexture.loadFromFile(BOSS_EASY_TEXTURE_PATH)) {
        exit(1);
    }
    if (!mediumBossTexture.loadFromFile(BOSS_MEDIUM_TEXTURE_PATH)) {
        exit(1);
    }
    if (!hardBossTexture.loadFromFile(BOSS_HARD_TEXTURE_PATH)) {
        exit(1);
    }
}

std::unique_ptr<Entity> BossSystem::spawnBoss(BossType type) const {
    // init
    int maxHp = 0;
    float size = 0.f;
    sf::Color color;
    int framesCount = BOSS_TEXTURE_FRAMES_COUNT;
    const sf::Texture *bossTexture = nullptr;

    switch (type) {
        case BossType::EASY:
            maxHp = EASY_BOSS_HP;
            size = BOSS_SIZE;
            color = BOSS_COLOR_DEFAULT;
            bossTexture = &easyBossTexture;
            break;
        case BossType::MEDIUM:
            maxHp = MEDIUM_BOSS_HP;
            size = BOSS_SIZE;
            color = BOSS_COLOR_TYPE_MEDIUM;
            framesCount = BOSS_TEXTURE_FRAMES_COUNT_MEDIUM;
            bossTexture = &mediumBossTexture;
            break;
        case BossType::HARD:
            maxHp = HARD_BOSS_HP;
            size = BOSS_SIZE;
            color = BOSS_COLOR_TYPE_HARD;
            bossTexture = &hardBossTexture;
            break;
    }

    auto boss = std::make_unique<Entity>();
    boss->addComponent(std::make_unique<Boss>(type, maxHp));
    boss->addComponent(std::make_unique<Health>(static_cast<float>(maxHp)));
    boss->addComponent(std::make_unique<Position>(WINDOW_CENTER_X, 0.f - BOSS_SIZE));
    boss->addComponent(std::make_unique<Velocity>());
    boss->addComponent(std::make_unique<BossPatternData>());

    if (bossTexture) {
        boss->addComponent(std::make_unique<Shape>(size, size, sf::Color::White));
        boss->addComponent(std::make_unique<Textured>(*bossTexture));
        boss->addComponent(std::make_unique<TextureAnimation>(
            framesCount,
            BOSS_TEXTURE_FRAME_SIDE,
            BOSS_TEXTURE_FRAME_SIDE,
            BOSS_TEXTURE_FRAME_DURATION
        ));
    } else {
        boss->addComponent(std::make_unique<Shape>(size, size, color));
    }

    return boss;
}
