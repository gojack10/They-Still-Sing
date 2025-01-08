#include "AudioSystem.hpp"
#include "../../config/AssetPaths.hpp"
#include "../../utils/Debug.hpp"
#include <fstream>
#include <filesystem>

namespace Engine {

void AudioSystem::initialize(const std::string& configPath) {
    std::cout << "AudioSystem: Loading config from absolute path: " << configPath << std::endl;
    
    // Load configuration
    std::ifstream configFile(configPath);
    if (!configFile.is_open()) {
        std::cerr << "Failed to open audio config file: " << configPath << std::endl;
        return;
    }
    
    // Clear existing audio data
    sounds.clear();
    music.clear();
    categoryVolumes.clear();
    
    try {
        config = nlohmann::json::parse(configFile);
        if (!config.is_object()) {
            std::cerr << "Invalid audio config format: Root must be an object" << std::endl;
            return;
        }
        std::cout << "AudioSystem: Successfully parsed config file" << std::endl;
        
        // Debug print entire config
        std::cout << "AudioSystem: Full config contents:" << std::endl;
        std::cout << config.dump(2) << std::endl;
        
        // Initialize category volumes
        if (!config.contains("categories")) {
            std::cerr << "Audio config missing 'categories' section" << std::endl;
            return;
        }
        
        for (const auto& [category, data] : config["categories"].items()) {
            if (!data.contains("volume")) {
                std::cerr << "Category " << category << " missing volume setting" << std::endl;
                continue;
            }
            float volume = data["volume"].get<float>();
            categoryVolumes[category] = volume;
            std::cout << "AudioSystem: Category '" << category << "' volume initialized to " << volume << std::endl;
        }
        
        // Load sounds
        if (config.contains("sounds")) {
            for (const auto& [name, data] : config["sounds"].items()) {
                SoundData soundData;
                std::string filePath = AssetPaths::resolvePath(data["file"].get<std::string>());
                std::cout << "AudioSystem: Loading sound '" << name << "' from " << filePath << std::endl;
                
                // Verify file exists
                if (!std::filesystem::exists(filePath)) {
                    std::cerr << "AudioSystem: Sound file does not exist: " << filePath << std::endl;
                    continue;
                }
                
                if (!soundData.buffer.loadFromFile(filePath)) {
                    std::cerr << "Failed to load sound: " << filePath << std::endl;
                    continue;
                }
                
                // Verify buffer loaded correctly
                if (soundData.buffer.getSampleCount() == 0) {
                    std::cerr << "AudioSystem: Error - Loaded buffer is empty for " << name << std::endl;
                    continue;
                }
                
                // Create and attach sound
                soundData.sound.setBuffer(soundData.buffer);
                
                // Verify sound has buffer
                if (!soundData.sound.getBuffer()) {
                    std::cerr << "AudioSystem: Error - Failed to attach buffer to sound for " << name << std::endl;
                    continue;
                }
                
                soundData.baseVolume = data["base_volume"];
                soundData.currentVolume = soundData.baseVolume;
                soundData.targetVolume = soundData.baseVolume;
                soundData.category = data["category"];
                
                std::cout << "AudioSystem: Sound '" << name << "' loaded successfully:" << std::endl;
                std::cout << "  - File: " << filePath << std::endl;
                std::cout << "  - Duration: " << soundData.buffer.getDuration().asSeconds() << "s" << std::endl;
                std::cout << "  - Sample Count: " << soundData.buffer.getSampleCount() << std::endl;
                std::cout << "  - Channel Count: " << soundData.buffer.getChannelCount() << std::endl;
                std::cout << "  - Sample Rate: " << soundData.buffer.getSampleRate() << " Hz" << std::endl;
                
                // Apply initial volume based on category
                if (auto it = categoryVolumes.find(soundData.category); it != categoryVolumes.end()) {
                    float finalVolume = (soundData.baseVolume * it->second) / 100.f;
                    soundData.sound.setVolume(finalVolume);
                    std::cout << "  - Initial Volume: " << finalVolume << std::endl;
                }
                
                // Store the sound data
                sounds[name] = std::move(soundData);
                
                // Verify sound was stored correctly
                if (auto it = sounds.find(name); it != sounds.end()) {
                    if (!it->second.sound.getBuffer()) {
                        std::cerr << "AudioSystem: Error - Buffer lost after storing sound!" << std::endl;
                    }
                }
            }
        }
        
        // Load music
        if (config.contains("music")) {
            for (const auto& [name, data] : config["music"].items()) {
                auto musicPtr = std::make_unique<MusicData>();
                std::string filePath = AssetPaths::resolvePath(data["file"].get<std::string>());
                std::cout << "AudioSystem: Loading music '" << name << "' from " << filePath << std::endl;
                
                if (!musicPtr->music.openFromFile(filePath)) {
                    std::cerr << "Failed to load music: " << filePath << std::endl;
                    continue;
                }
                
                musicPtr->baseVolume = data["base_volume"];
                musicPtr->category = data["category"];
                
                std::cout << "AudioSystem: Music '" << name << "' base volume: " << musicPtr->baseVolume 
                         << ", category: " << musicPtr->category << std::endl;
                
                // Apply initial volume based on category
                if (auto it = categoryVolumes.find(musicPtr->category); it != categoryVolumes.end()) {
                    float finalVolume = (musicPtr->baseVolume * it->second) / 100.f;
                    musicPtr->music.setVolume(finalVolume);
                    std::cout << "AudioSystem: Setting initial music '" << name 
                             << "' volume to " << finalVolume 
                             << " (base: " << musicPtr->baseVolume 
                             << " * category: " << it->second << ")" << std::endl;
                } else {
                    std::cerr << "Warning: Music '" << name << "' has unknown category: " 
                             << musicPtr->category << std::endl;
                }
                
                if (data.contains("loop")) {
                    musicPtr->music.setLoop(data["loop"]);
                }
                
                music[name] = std::move(musicPtr);
                std::cout << "AudioSystem: Successfully loaded music '" << name << "'" << std::endl;
            }
        }
        
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return;
    } catch (const std::exception& e) {
        std::cerr << "Error during audio initialization: " << e.what() << std::endl;
        return;
    }
    
    // Final verification of loaded data
    std::cout << "\nAudioSystem: Initialization Summary:" << std::endl;
    std::cout << "Categories loaded: " << categoryVolumes.size() << std::endl;
    for (const auto& [category, volume] : categoryVolumes) {
        std::cout << "  - " << category << ": " << volume << std::endl;
    }
    std::cout << "Sounds loaded: " << sounds.size() << std::endl;
    std::cout << "Music tracks loaded: " << music.size() << std::endl;
}

void AudioSystem::playSound(const std::string& name) {
    if (debugEnabled) std::cout << "AudioSystem: Attempting to play sound '" << name << "'" << std::endl;
    
    if (auto it = sounds.find(name); it != sounds.end()) {
        if (debugEnabled) std::cout << "AudioSystem: Found sound '" << name << "' in loaded sounds" << std::endl;
        
        // Verify buffer is valid
        if (!it->second.buffer.getSampleCount()) {
            std::cerr << "AudioSystem: Error - Sound buffer is empty!" << std::endl;
            return;
        }
        
        // Print current sound properties if debug enabled
        if (debugEnabled) {
            std::cout << "AudioSystem: Sound properties before play:" << std::endl;
            std::cout << "  - Base Volume: " << it->second.baseVolume << std::endl;
            std::cout << "  - Current Volume: " << it->second.currentVolume << std::endl;
            std::cout << "  - Category: " << it->second.category << std::endl;
            std::cout << "  - Buffer Duration: " << it->second.buffer.getDuration().asSeconds() << "s" << std::endl;
            std::cout << "  - Buffer Sample Count: " << it->second.buffer.getSampleCount() << std::endl;
            std::cout << "  - Buffer Channel Count: " << it->second.buffer.getChannelCount() << std::endl;
        }
        
        // Verify sound has buffer attached
        if (!it->second.sound.getBuffer()) {
            std::cerr << "AudioSystem: Error - Sound has no buffer attached!" << std::endl;
            // Try re-attaching buffer
            it->second.sound.setBuffer(it->second.buffer);
            if (debugEnabled) std::cout << "AudioSystem: Re-attached buffer to sound" << std::endl;
        }
        
        // Update volume before playing
        updateSoundProperties(it->second);
        
        // Print final volume and status if debug enabled
        if (debugEnabled) {
            std::cout << "AudioSystem: Final volume after properties update: " << it->second.sound.getVolume() << std::endl;
            std::cout << "AudioSystem: Sound buffer address: " << &(it->second.buffer) << std::endl;
            std::cout << "AudioSystem: Sound object address: " << &(it->second.sound) << std::endl;
        }
        
        // Play the sound
        it->second.sound.play();
        if (debugEnabled) std::cout << "AudioSystem: Started playing sound '" << name << "'" << std::endl;
        
    } else {
        std::cerr << "AudioSystem: Sound '" << name << "' not found in loaded sounds!" << std::endl;
        if (debugEnabled) {
            std::cout << "AudioSystem: Currently loaded sounds:" << std::endl;
            for (const auto& [soundName, _] : sounds) {
                std::cout << "  - " << soundName << std::endl;
            }
        }
    }
}

void AudioSystem::playMusic(const std::string& name) {
    std::cout << "AudioSystem: Attempting to play music " << name << std::endl;
    if (auto it = music.find(name); it != music.end()) {
        // Update volume before playing
        if (auto catIt = categoryVolumes.find(it->second->category); catIt != categoryVolumes.end()) {
            float finalVolume = (it->second->baseVolume * catIt->second) / 100.f;
            it->second->music.setVolume(finalVolume);
        }
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
    // Only clamp volume to prevent negative values
    float clampedVolume = std::max(0.f, volume);
    categoryVolumes[category] = clampedVolume;
    
    // Update all sounds in this category
    for (auto& [name, soundData] : sounds) {
        if (soundData.category == category) {
            updateSoundProperties(soundData);
        }
    }
    
    // Update all music in this category
    for (auto& [name, musicPtr] : music) {
        if (musicPtr->category == category) {
            float finalVolume = (musicPtr->baseVolume * clampedVolume) / 100.f;
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
        // Only prevent negative values, allow volumes above 100
        finalVolume = std::max(0.f, (finalVolume * it->second) / 100.f);
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