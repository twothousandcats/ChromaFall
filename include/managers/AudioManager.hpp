#pragma once

#include <string>
#include <unordered_map>
#include <SFML/Audio.hpp>

#include "config/AudioConfig.hpp"

class AudioManager {
public:
    // Singleton access
    static AudioManager &getInstance();

    // SFX
    void loadSound(const std::string &name, const std::string &filepath);

    void playSound(const std::string &name);

    // Music
    void playMusic(const std::string &filepath);

    void stopMusic();

    void pauseMusic();

    void resumeMusic();

    void setMusicVolume(float volume); // 0-100
    void setSoundVolume(float volume); // 0-100

    // запрет копирования TODO: разобраться
    AudioManager(const AudioManager &) = delete;

    AudioManager &operator=(const AudioManager &) = delete;

private:
    AudioManager() = default;

    ~AudioManager() = default;

    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer> > soundBuffers;
    std::vector<std::unique_ptr<sf::Sound> > activeSounds; // одновременные SFX
    std::unique_ptr<sf::Music> currentMusic = std::make_unique<sf::Music>();

    float currentSoundVolume = AUDIO_SFX_VOLUME_MAX;
    float currentMusicVolume = AUDIO_MUSIC_VOLUME_MAX;

    void cleanupFinishedSounds();
};
