#pragma once
#include <string>
#include <unordered_map>
#include <SFML/Audio.hpp>

#include "config/AudioConfig.hpp"

class AudioManager {
public:
    // Загрузка и воспроизведение по имени один раз
    void loadSound(const std::string &name, const std::string &filepath);

    void playSound(const std::string &name);

    // Музыка загрузка и управление
    void playMusic(const std::string &filepath);

    void stopMusic();

    void setMusicVolume(float volume);

    void setSoundVolume(float volume);

    // Запрет копирования
    AudioManager(const AudioManager &) = delete;

    AudioManager &operator=(const AudioManager &) = delete;

private:
    ~AudioManager() = default;

    // TODO: возможно надо будет использовать пул звуков
    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
    sf::Sound currentSound;
    sf::Music music;
    float soundVolume = SFX_VOLUME;
    float musicVolume = MUSIC_VOLUME;
};
