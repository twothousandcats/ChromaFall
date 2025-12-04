#include "systems/WaveSystem.hpp"

#include "config/WaveConfig.hpp"

WaveSystem::WaveSystem(
    const int totalWaves,
    const int baseKillsPerWave
)
    : totalWaves(totalWaves)
      , baseKillsPerWave(baseKillsPerWave) {
}

int WaveSystem::getRequiredKillsForCurrentWave() const {
    return baseKillsPerWave * currentWave;
}

void WaveSystem::advanceToNextWave() {
    currentWave++;
    killedAsteroidsInCurrentWave = 0;
    phase = WavePhase::ASTEROIDS;
}

void WaveSystem::update(
    const int asteroidsDestroyedThisFrame,
    const float deltaTime
) {
    if (isVictory()) {
        return;
    }

    if (phase == WavePhase::COOLDOWN) {
        waveCooldownTimer -= deltaTime;
        if (waveCooldownTimer <= 0.f) {
            advanceToNextWave(); // reset
        }
        return;
    }

    if (phase == WavePhase::ASTEROIDS) {
        killedAsteroidsInCurrentWave += asteroidsDestroyedThisFrame;
        if (killedAsteroidsInCurrentWave >= getRequiredKillsForCurrentWave()) {
            phase = WavePhase::BOSS; // boss phase
        }
    }
}
