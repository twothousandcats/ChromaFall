#pragma once
#include "config/WaveConfig.hpp"

enum class WavePhase {
    ASTEROIDS,
    BOSS,
    COOLDOWN
};

class WaveSystem {
public:
    explicit WaveSystem(int totalWaves, int baseKillsPerWave);

    void update(int asteroidsDestroyedThisFrame, float deltaTime);

    void onBossDefeated() {
        phase = WavePhase::COOLDOWN;
        waveCooldownTimer = WAVE_COOLDOWN_DURATION;
    }

    [[nodiscard]] bool isVictory() const {
        return currentWave > totalWaves;
    }

    [[nodiscard]] int getCurrentWave() const { return currentWave; }
    [[nodiscard]] int getTotalWaves() const { return totalWaves; }

    [[nodiscard]] bool shouldSpawnAsteroids() const {
        if (phase == WavePhase::ASTEROIDS) return true;
        if (phase == WavePhase::BOSS) return currentWave == 1; // только Easy
        return false;
    }

    [[nodiscard]] bool isAsteroidPhase() const { return phase == WavePhase::ASTEROIDS; }
    [[nodiscard]] bool isBossPhase() const { return phase == WavePhase::BOSS; }

private:
    WavePhase phase = WavePhase::ASTEROIDS;
    int currentWave = 1;
    int totalWaves;
    int killedAsteroidsInCurrentWave = 0;
    int baseKillsPerWave;

    float waveCooldownTimer = 0.f;

    [[nodiscard]] int getRequiredKillsForCurrentWave() const;

    void advanceToNextWave();
};
