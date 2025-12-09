#include "systems/ExperienceSystem.hpp"

#include "components/Experience.hpp"
#include "components/Level.hpp"
#include "config/LevelConfig.hpp"
#include "entities/Entity.hpp"

void ExperienceSystem::onAsteroidsDestroyed(
    const int count
) {
    pendingExp += count * LEVEL_ASTEROID_EXP_COST;
}


void ExperienceSystem::onBossDefeated(
    const BossType type
) {
    switch (type) {
        case BossType::EASY: pendingExp += LEVEL_BOSS_EASY;
            break;
        case BossType::MEDIUM: pendingExp += LEVEL_BOSS_MEDIUM;
            break;
        case BossType::HARD: pendingExp += LEVEL_BOSS_HARD;
            break;
    }
}


bool ExperienceSystem::processLevelUps(
    Entity *player
    ) {if (!player || pendingExp <= 0) {
        pendingExp = 0;
        return false;
    }

    auto* exp = player->getComponent<Experience>();
    auto* level = player->getComponent<Level>();
    if (!exp || !level) {
        pendingExp = 0;
        return false;
    }

    exp->value += pendingExp;
    pendingExp = 0;
    levelUpTriggered = false;

    while (exp->value >= exp->neededValue) {
        exp->value -= exp->neededValue;
        level->value++;
        exp->neededValue = LEVEL_BASE_NEEDED_COUNT_EXP + (level->value - 1) * LEVEL_NEEDED_COUNT_EXP_ACC;
        levelUpTriggered = true;
    }

    return levelUpTriggered;
}

void ExperienceSystem::reset() {
    pendingExp = 0;
    levelUpTriggered = false;
}