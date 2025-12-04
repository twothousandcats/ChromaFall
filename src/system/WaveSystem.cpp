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
    isWaveCompleted = false;
}

void WaveSystem::update(
    const int asteroidsDestroyedThisFrame,
    const float deltaTime
) {
    if (isVictory() || isWaveCompleted) {
        if (isWaveCompleted) {
            waveCooldownTimer -= deltaTime;
            if (waveCooldownTimer <= 0.f) {
                advanceToNextWave();
            }
        }
        return;
    }

    killedAsteroidsInCurrentWave += asteroidsDestroyedThisFrame;

    if (killedAsteroidsInCurrentWave >= getRequiredKillsForCurrentWave()) {
        isWaveCompleted = true;
        waveCooldownTimer = WAVE_COOLDOWN_DURATION;
    }
}