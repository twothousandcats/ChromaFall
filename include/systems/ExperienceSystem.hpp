#pragma once
#include "components/Boss.hpp"

class Entity;

// подсчет опыта за кадр
class ExperienceSystem {
public:
    void onAsteroidsDestroyed(int count);

    void onBossDefeated(BossType type);

    bool processLevelUps(Entity *player);

    void reset();

private:
    int pendingExp = 0;
    bool levelUpTriggered = false;
};
