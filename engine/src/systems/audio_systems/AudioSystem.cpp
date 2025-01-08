#include "AudioSystem.hpp"
#include "../../config/AssetPaths.hpp"
#include "../../utils/Debug.hpp"
#include <fstream>

namespace Engine {

void AudioSystem::initialize(const std::string& configPath) {
    std::cout << "AudioSystem: Initializing with config: " << configPath << std::endl;
    
    // Load configuration
    std::ifstream configFile(configPath);
    if (!configFile.is_open()) {
        std::cerr << "Failed to open audio config file: " << configPath << std::endl;
        return;
    }
    
    config = nlohmann::json::parse(configFile);
    std::cout << "AudioSystem: Successfully parsed config file" << std::endl;
    
    // Initialize category volumes
    for (const auto& [category, data] : config["categories"].items()) {
        categoryVolumes[category] = data["volume"];
        std::cout << "AudioSystem: Set " << category << " volume to " << data["volume"] << std::endl;
    }
    
    // Load sounds
    for (const auto& [name, data] : config["sounds"].items()) {
        SoundData soundData;
        std::string filePath = AssetPaths::resolvePath(data["file"].get<std::string>());
        std::cout << "AudioSystem: Loading sound " << name << " from " << filePath << std::endl;
        
        if (!soundData.buffer.loadFromFile(filePath)) {
            std::cerr << "Failed to load sound: " << filePath << std::endl;
            continue;
        }
        
        soundData.sound.setBuffer(soundData.buffer);
        soundData.baseVolume = data["base_volume"];
        soundData.currentVolume = soundData.baseVolume;
        soundData.targetVolume = soundData.baseVolume;
        soundData.category = data["category"];
        
        if (data.contains("spatial")) {
            soundData.spatial = data["spatial"];
            if (data.contains("min_volume")) {
                soundData.minVolume = data["min_volume"];
            }
        }
        
        sounds[name] = std::move(soundData);
        std::cout << "AudioSystem: Successfully loaded sound " << name << std::endl;
    }
    
    // Load music
    for (const auto& [name, data] : config["music"].items()) {
        auto musicPtr = std::make_unique<MusicData>();
        std::string filePath = AssetPaths::resolvePath(data["file"].get<std::string>());
        std::cout << "AudioSystem: Loading music " << name << " from " << filePath << std::endl;
        
        if (!musicPtr->music.openFromFile(filePath)) {
            std::cerr << "Failed to load music: " << filePath << std::endl;
            continue;
        }
        
        musicPtr->baseVolume = data["base_volume"];
        musicPtr->category = data["category"];
        
        if (data.contains("loop")) {
            musicPtr->music.setLoop(data["loop"]);
        }
        
        music[name] = std::move(musicPtr);
        std::cout << "AudioSystem: Successfully loaded music " << name << std::endl;
    }
}

void AudioSystem::playSound(const std::string& name) {
    if (auto it = sounds.find(name); it != sounds.end()) {
        it->second.sound.play();
    }
}

void AudioSystem::playMusic(const std::string& name) {
    std::cout << "AudioSystem: Attempting to play music " << name << std::endl;
    if (auto it = music.find(name); it != music.end()) {
        it->second->music.play();
        std::cout << "AudioSystem: Started playing music " << name << std::endl;
    } else {
        std::cerr << "AudioSystem: Music " << name << " not found!" << std::endl;
    }
}

void AudioSystem::stopSound(const std::string& name) {
    if (auto it = sounds.find(name); it != sounds.end()) {
        it->second.sound.stop();
    }
}

void AudioSystem::stopMusic(const std::string& name) {
    std::cout << "AudioSystem: Attempting to stop music " << name << std::endl;
    if (auto it = music.find(name); it != music.end()) {
        it->second->music.stop();
        std::cout << "AudioSystem: Stopped music " << name << std::endl;
    } else {
        std::cerr << "AudioSystem: Music " << name << " not found!" << std::endl;
    }
}

void AudioSystem::updateVirtualPosition(const std::string& name, float angle) {
    if (auto it = sounds.find(name); it != sounds.end()) {
        it->second.virtualAngle = normalizeAngle(angle);
        updateSoundProperties(it->second);
    }
}

void AudioSystem::setPlayerRotation(float angle) {
    playerRotation = normalizeAngle(angle);
    
    // Update all spatial sounds
    for (auto& [name, soundData] : sounds) {
        if (soundData.spatial) {
            updateSoundProperties(soundData);
        }
    }
}

void AudioSystem::setCategoryVolume(const std::string& category, float volume) {
    categoryVolumes[category] = volume;
    
    // Update all sounds in this category
    for (auto& [name, soundData] : sounds) {
        if (soundData.category == category) {
            updateSoundProperties(soundData);
        }
    }
    
    // Update all music in this category
    for (auto& [name, musicPtr] : music) {
        if (musicPtr->category == category) {
            float finalVolume = (musicPtr->baseVolume * volume) / 100.f;
            musicPtr->music.setVolume(finalVolume);
        }
    }
}

void AudioSystem::setSoundVolume(const std::string& name, float volume) {
    if (auto it = sounds.find(name); it != sounds.end()) {
        it->second.baseVolume = volume;
        updateSoundProperties(it->second);
    }
}

void AudioSystem::fadeIn(const std::string& name, float duration) {
    if (auto it = sounds.find(name); it != sounds.end()) {
        it->second.fading = true;
        it->second.fadeTime = 0.f;
        it->second.fadeDuration = duration;
        it->second.fadeStartVolume = 0.f;
        it->second.fadeTargetVolume = it->second.baseVolume;
        it->second.sound.setVolume(0.f);
        it->second.sound.play();
    }
}

void AudioSystem::fadeOut(const std::string& name, float duration) {
    if (auto it = sounds.find(name); it != sounds.end()) {
        it->second.fading = true;
        it->second.fadeTime = 0.f;
        it->second.fadeDuration = duration;
        it->second.fadeStartVolume = it->second.currentVolume;
        it->second.fadeTargetVolume = 0.f;
    }
}

void AudioSystem::update(float deltaTime) {
    // Update fading sounds
    for (auto& [name, soundData] : sounds) {
        if (soundData.fading) {
            soundData.fadeTime += deltaTime;
            
            if (soundData.fadeTime >= soundData.fadeDuration) {
                soundData.fading = false;
                soundData.currentVolume = soundData.fadeTargetVolume;
                if (soundData.fadeTargetVolume <= 0.f) {
                    soundData.sound.stop();
                }
            } else {
                float t = soundData.fadeTime / soundData.fadeDuration;
                soundData.currentVolume = soundData.fadeStartVolume + 
                    (soundData.fadeTargetVolume - soundData.fadeStartVolume) * t;
            }
            
            updateSoundProperties(soundData);
        }
    }

    // Check for status changes and notify callbacks
    checkAndNotifyStatusChanges();
}

void AudioSystem::checkAndNotifyStatusChanges() {
    // Check music status changes
    for (auto& [name, musicPtr] : music) {
        auto currentStatus = musicPtr->music.getStatus();
        if (currentStatus != musicPtr->lastStatus) {
            if (currentStatus == sf::SoundSource::Playing && onMusicStart) {
                onMusicStart(name);
            }
            else if (currentStatus == sf::SoundSource::Stopped && onMusicStop) {
                onMusicStop(name);
            }
            musicPtr->lastStatus = currentStatus;
        }
    }

    // Check sound status changes
    for (auto& [name, soundData] : sounds) {
        auto currentStatus = soundData.sound.getStatus();
        if (currentStatus != soundData.lastStatus) {
            if (currentStatus == sf::SoundSource::Playing && onSoundStart) {
                onSoundStart(name);
            }
            else if (currentStatus == sf::SoundSource::Stopped && onSoundStop) {
                onSoundStop(name);
            }
            soundData.lastStatus = currentStatus;
        }
    }
}

bool AudioSystem::isMusicPlaying(const std::string& name) const {
    if (auto it = music.find(name); it != music.end()) {
        return it->second->music.getStatus() == sf::SoundSource::Playing;
    }
    return false;
}

bool AudioSystem::isSoundPlaying(const std::string& name) const {
    if (auto it = sounds.find(name); it != sounds.end()) {
        return it->second.sound.getStatus() == sf::SoundSource::Playing;
    }
    return false;
}

sf::SoundSource::Status AudioSystem::getMusicStatus(const std::string& name) const {
    if (auto it = music.find(name); it != music.end()) {
        return it->second->music.getStatus();
    }
    return sf::SoundSource::Stopped;
}

sf::SoundSource::Status AudioSystem::getSoundStatus(const std::string& name) const {
    if (auto it = sounds.find(name); it != sounds.end()) {
        return it->second.sound.getStatus();
    }
    return sf::SoundSource::Stopped;
}

void AudioSystem::updateSoundProperties(SoundData& soundData) {
    float finalVolume = soundData.currentVolume;
    float pan = 0.f;
    
    // Apply spatial audio if enabled
    if (soundData.spatial) {
        float relativeAngle = normalizeAngle(soundData.virtualAngle - playerRotation);
        pan = calculatePanning(relativeAngle);
        finalVolume *= calculateVolume(relativeAngle, soundData.minVolume);
    }
    
    // Apply category volume
    if (auto it = categoryVolumes.find(soundData.category); it != categoryVolumes.end()) {
        finalVolume = (finalVolume * it->second) / 100.f;
    }
    
    soundData.sound.setVolume(finalVolume);
    // Since SFML doesn't have setPan, we'll simulate panning by adjusting the volume
    if (pan < 0) {
        soundData.sound.setRelativeToListener(true);
        soundData.sound.setPosition(-pan * 100.f, 0.f, 0.f);
    } else {
        soundData.sound.setRelativeToListener(true);
        soundData.sound.setPosition(pan * 100.f, 0.f, 0.f);
    }
}

float AudioSystem::calculatePanning(float relativeAngle) {
    // Convert angle to radians and calculate pan
    float radians = relativeAngle * PI / 180.f;
    return std::sin(radians);  // Returns -1 to 1
}

float AudioSystem::calculateVolume(float relativeAngle, float minVolume) {
    // Calculate volume falloff based on angle
    float normalizedVolume = std::cos(relativeAngle * PI / 180.f);
    normalizedVolume = (normalizedVolume + 1.f) / 2.f;  // Convert from -1,1 to 0,1
    
    // Apply minimum volume
    float volumeRange = 1.f - (minVolume / 100.f);
    return (minVolume / 100.f) + (normalizedVolume * volumeRange);
}

float AudioSystem::normalizeAngle(float angle) {
    // Normalize angle to -180 to 180 degrees
    angle = std::fmod(angle, 360.f);
    if (angle > 180.f) {
        angle -= 360.f;
    } else if (angle < -180.f) {
        angle += 360.f;
    }
    return angle;
}

} // namespace Engine 