#include "AnimationManager.hpp"
#include "../../config/AssetPaths.hpp"

bool AnimationManager::loadAnimation(const std::string& name, const std::string& path, bool looping) {
    auto animation = std::make_unique<Animation>();
    
    if (!animation->loadFromDirectory(path)) {
        return false;
    }
    
    animation->setFrameTime(FRAME_TIME);
    animation->setLooping(looping);
    
    animations[name] = std::move(animation);
    return true;
}

Animation* AnimationManager::getAnimation(const std::string& name) {
    auto it = animations.find(name);
    if (it != animations.end()) {
        return it->second.get();
    }
    return nullptr;
}

void AnimationManager::update(float deltaTime) {
    for (auto& [name, animation] : animations) {
        animation->update(deltaTime);
    }
} 