#pragma once

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <deque>
#include <memory>
#include <mutex>

class Animation {
public:
    Animation();
    ~Animation();
    
    bool loadFromDirectory(const std::string& path, const std::string& extension = ".jpg");
    bool loadFrame(size_t index);
    void update(float deltaTime);
    sf::Sprite& getCurrentFrame();
    
    void play();
    void pause();
    void stop();
    void reset();
    
    bool isPlaying() const { return playing; }
    bool hasFrames() const { return !framePaths.empty(); }
    size_t getFrameCount() const { return framePaths.size(); }
    
    void setFrameTime(float time) { frameTime = time; }
    void setLooping(bool loop) { isLooping = loop; }
    void setMaxLoadedFrames(size_t max) { maxLoadedFrames = max; }
    
private:
    std::shared_ptr<sf::Texture> ensureFrameLoaded(size_t index);
    void maintainFrameWindow();
    
    float frameTime;
    float currentTime;
    size_t currentFrame;
    bool playing;
    bool isLooping;
    size_t totalMemoryUsage;
    size_t maxLoadedFrames;
    bool isInitialLoad;
    
    std::recursive_mutex frameMutex;
    std::deque<std::pair<size_t, std::shared_ptr<sf::Texture>>> loadedFrames;
    std::vector<std::filesystem::path> framePaths;
    std::shared_ptr<sf::Texture> currentTexture;  // Keep as shared_ptr
    sf::Sprite currentSprite;
    
    static constexpr size_t DEFAULT_MAX_FRAMES = 60;  // Default to 2 seconds at 30 FPS
}; 