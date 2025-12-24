#include "managers/AudioManager.hpp"

#include <iostream>

AudioManager &AudioManager::getInstance() {
    static AudioManager manager;
    return manager;
}

void AudioManager::loadSound(
    const std::string &name,
    const std::string &filepath
) {
    // загружен?
    if (soundBuffers.find(name) != soundBuffers.end()) {
        return;
    }

    auto buffer = std::make_unique<sf::SoundBuffer>();
    if (!buffer->loadFromFile(filepath)) {
        std::cerr << "Failed to load sound buffer: " << filepath << std::endl;
        return;
    }

    soundBuffers.emplace(name, std::move(buffer));
}

void AudioManager::playSound(
    const std::string &name
) {
    auto it = soundBuffers.find(name);
    if (it == soundBuffers.end()) {
        std::cerr << "Sound not loaded: " << name << std::endl;
        return;
    }

    cleanupFinishedSounds();

    auto sound = std::make_unique<sf::Sound>(*it->second);
    sound->setVolume(currentSoundVolume);
    sound->play();
    activeSounds.push_back(std::move(sound));
}

void AudioManager::playMusic(
    const std::string &filepath
) {
    if (!currentMusic->openFromFile(filepath)) {
        std::cerr << "Failed to open music: " << filepath << std::endl;
        return;
    }

    currentMusic->setLooping(true);
    currentMusic->setVolume(currentMusicVolume);
    currentMusic->play();
}

void AudioManager::stopMusic() {
    currentMusic->stop();
}

void AudioManager::pauseMusic() {
    currentMusic->pause();
}

void AudioManager::resumeMusic() {
    currentMusic->play();
}

void AudioManager::setMusicVolume(
    float volume
) {
    currentMusicVolume = std::clamp(volume, AUDIO_MUSIC_VOLUME_MIN, AUDIO_MUSIC_VOLUME_MAX);
    currentMusic->setVolume(currentMusicVolume);
}

void AudioManager::setSoundVolume(
    float volume
) {
    currentSoundVolume = std::clamp(volume, AUDIO_SFX_VOLUME_MIN, AUDIO_SFX_VOLUME_MAX);
}

void AudioManager::cleanupFinishedSounds() {
    activeSounds.erase(
        std::remove_if(activeSounds.begin(), activeSounds.end(),
                       [](const auto& sound) {
                           return sound->getStatus() == sf::Sound::Status::Stopped;
                       }),
        activeSounds.end()
    );
}
