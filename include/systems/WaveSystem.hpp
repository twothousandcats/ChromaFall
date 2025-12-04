#pragma once

class WaveSystem {
public:
    explicit WaveSystem(int totalWaves, int baseKillsPerWave);

    void update(int asteroidsDestroyedThisFrame, float deltaTime);

    [[nodiscard]] bool isWaveActive() const { return !isWaveCompleted; }
    [[nodiscard]] bool isVictory() const { return currentWave > totalWaves; }
    [[nodiscard]] int getCurrentWave() const { return currentWave; }
    [[nodiscard]] int getTotalWaves() const { return totalWaves; }
    [[nodiscard]] bool shouldSpawnAsteroids() const { return currentWave <= totalWaves; }

private:
    int currentWave = 1;
    int totalWaves;
    int killedAsteroidsInCurrentWave = 0;
    int baseKillsPerWave;

    bool isWaveCompleted = false;
    float waveCooldownTimer = 0.f;

    [[nodiscard]] int getRequiredKillsForCurrentWave() const;

    void advanceToNextWave();
};