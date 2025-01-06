#pragma once

#include "Animation.hpp"
#include <unordered_map>
#include <memory>
#include <string>

class AnimationManager {
public:
    static AnimationManager& getInstance() {
        static AnimationManager instance;
        return instance;
    }
    
    // Load an animation sequence from a directory
    bool loadAnimation(const std::string& name, const std::string& path, bool looping = true);
    
    // Get animation by name
    Animation* getAnimation(const std::string& name);
    
    // Update all playing animations
    void update(float deltaTime);
    
private:
    AnimationManager() = default;
    ~AnimationManager() = default;
    AnimationManager(const AnimationManager&) = delete;
    AnimationManager& operator=(const AnimationManager&) = delete;
    
    static constexpr float FRAME_TIME = 1.0f/30.0f;  // Hardcoded 30 FPS
    std::unordered_map<std::string, std::unique_ptr<Animation>> animations;
}; 