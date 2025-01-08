#pragma once

#include <SFML/Audio.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <map>
#include <memory>
#include <cmath>
#include <functional>

namespace Engine {

class AudioSystem {
public:
    // Callback types
    using AudioCallback = std::function<void(const std::string&)>;

    static AudioSystem& getInstance() {
        static AudioSystem instance;
        return instance;
    }

    void initialize(const std::string& configPath);
    
    // Sound playback
    void playSound(const std::string& name);
    void playMusic(const std::string& name);
    void stopSound(const std::string& name);
    void stopMusic(const std::string& name);
    
    // Virtual 3D audio
    void updateVirtualPosition(const std::string& name, float angle);
    void setPlayerRotation(float angle); // Angle in degrees, 0 is forward
    
    // Volume control
    void setCategoryVolume(const std::string& category, float volume);
    void setSoundVolume(const std::string& name, float volume);
    
    // Fade effects
    void fadeIn(const std::string& name, float duration);
    void fadeOut(const std::string& name, float duration);
    
    // Update system
    void update(float deltaTime);

    // Audio state callbacks
    void setMusicStartCallback(AudioCallback callback) { onMusicStart = callback; }
    void setMusicStopCallback(AudioCallback callback) { onMusicStop = callback; }
    void setSoundStartCallback(AudioCallback callback) { onSoundStart = callback; }
    void setSoundStopCallback(AudioCallback callback) { onSoundStop = callback; }

    // Audio state queries
    bool isMusicPlaying(const std::string& name) const;
    bool isSoundPlaying(const std::string& name) const;
    sf::SoundSource::Status getMusicStatus(const std::string& name) const;
    sf::SoundSource::Status getSoundStatus(const std::string& name) const;

private:
    AudioSystem() = default;
    ~AudioSystem() = default;
    AudioSystem(const AudioSystem&) = delete;
    AudioSystem& operator=(const AudioSystem&) = delete;

    struct SoundData {
        sf::SoundBuffer buffer;
        sf::Sound sound;
        float baseVolume = 100.f;
        float currentVolume = 100.f;
        float targetVolume = 100.f;
        float virtualAngle = 0.f;  // Angle relative to player's forward direction
        bool spatial = false;      // Whether sound uses virtual positioning
        float minVolume = 0.f;     // Minimum volume for spatial sounds
        std::string category;
        bool fading = false;
        float fadeTime = 0.f;
        float fadeDuration = 0.f;
        float fadeStartVolume = 0.f;
        float fadeTargetVolume = 0.f;
        sf::SoundSource::Status lastStatus = sf::SoundSource::Stopped;
    };

    struct MusicData {
        sf::Music music;
        float baseVolume = 100.f;
        std::string category;
        sf::SoundSource::Status lastStatus = sf::SoundSource::Stopped;
    };

    // Helper functions
    void updateSoundProperties(SoundData& soundData);
    float calculatePanning(float relativeAngle);
    float calculateVolume(float relativeAngle, float minVolume);
    float normalizeAngle(float angle);
    void checkAndNotifyStatusChanges();

    nlohmann::json config;
    std::map<std::string, SoundData> sounds;
    std::map<std::string, std::unique_ptr<MusicData>> music;
    std::map<std::string, float> categoryVolumes;
    
    float playerRotation = 0.f;  // Current player rotation in degrees
    const float PI = 3.14159265359f;

    // Callback functions
    AudioCallback onMusicStart;
    AudioCallback onMusicStop;
    AudioCallback onSoundStart;
    AudioCallback onSoundStop;
};

} // namespace Engine
