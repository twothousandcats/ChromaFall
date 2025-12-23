#include <iostream>
#include "managers/AudioManager.hpp"

void AudioManager::loadSound(const std::string& name, const std::string& filepath) {
    if (soundBuffers.find(name) == soundBuffers.end()) {
        if (!soundBuffers[name].loadFromFile(filepath)) {
            std::cerr << "Failed to load sound: " << filepath << std::endl;
        }
    }
}

void AudioManager::playSound(const std::string& name) {
    if (soundBuffers.find(name) == soundBuffers.end()) {
        currentSound.setBuffer(soundBuffers[name]);
        currentSound.setVolume(soundVolume);
        currentSound.play();
    }
}

void AudioManager::playMusic(const std::string& filepath) {
    if (music.openFromFile(filepath)) {
        music.setLoopPoints({
            sf::milliseconds(500),
            sf::seconds(4)
        }); // TODO: отладка
        music.setVolume(musicVolume);
        music.play();
    } else {
        std::cerr << "Failed to load music: " << filepath << std::endl;
    }
}

void AudioManager::stopMusic() {
    music.stop();
}

void AudioManager::setMusicVolume(float volume) {
    musicVolume = volume;
    music.setVolume(musicVolume);
}

void AudioManager::setSoundVolume(float volume) {
    soundVolume = volume;
}