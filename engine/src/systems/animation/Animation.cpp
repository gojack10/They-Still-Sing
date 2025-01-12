#include "Animation.hpp"
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <numeric>

Animation::Animation()
    : frameTime(1.0f/30.0f)  // Default 30 FPS
    , currentTime(0.0f)
    , currentFrame(0)
    , playing(false)
    , isLooping(true)
    , totalMemoryUsage(0)
    , maxLoadedFrames(DEFAULT_MAX_FRAMES)
    , isInitialLoad(true)
{
    std::cout << "Animation: Constructor called" << std::endl;
    currentSprite.setPosition(0, 0);
}

Animation::~Animation() {
    std::cout << "Animation: Destructor called, cleaning up " << loadedFrames.size() << " frames" << std::endl;
    std::lock_guard<std::recursive_mutex> lock(frameMutex);
    loadedFrames.clear();
    framePaths.clear();
    currentTexture.reset();
    std::cout << "Animation: All frames cleaned up" << std::endl;
}

bool Animation::loadFromDirectory(const std::string& path, const std::string& extension) {
    namespace fs = std::filesystem;
    
    try {
        if (!fs::exists(path)) {
            std::cerr << "Animation::loadFromDirectory: Directory does not exist: " << path << std::endl;
            return false;
        }

        if (!fs::is_directory(path)) {
            std::cerr << "Animation::loadFromDirectory: Path is not a directory: " << path << std::endl;
            return false;
        }
        
        // Clear existing frames and reset state
        {
            std::cout << "Animation::loadFromDirectory: Clearing existing frames" << std::endl;
            stop();
            loadedFrames.clear();
            framePaths.clear();
            currentTexture.reset();
            totalMemoryUsage = 0;
            isInitialLoad = true;  // Set initial load flag
        }
        
        // Get all files with matching extension
        std::vector<fs::path> files;
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.path().extension() == extension) {
                files.push_back(entry.path());
            }
        }
        
        if (files.empty()) {
            std::cerr << "Animation::loadFromDirectory: No files found with extension " << extension << " in " << path << std::endl;
            return false;
        }
        
        // Sort files by name to ensure correct sequence
        std::sort(files.begin(), files.end());
        
        // Store paths
        {
            framePaths = std::move(files);
        }
        
        std::cout << "Animation::loadFromDirectory: Found " << framePaths.size() << " frames" << std::endl;
        
        // Load initial frame to get dimensions
        auto firstTexture = ensureFrameLoaded(0);
        if (!firstTexture) {
            std::cerr << "Animation::loadFromDirectory: Failed to load first frame" << std::endl;
            return false;
        }
        
        auto size = firstTexture->getSize();
        std::cout << "Animation::loadFromDirectory: First frame size: " << size.x << "x" << size.y << std::endl;
        
        // Initial loading phase complete
        isInitialLoad = false;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Animation::loadFromDirectory: Exception: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<sf::Texture> Animation::ensureFrameLoaded(size_t index) {
    std::lock_guard<std::recursive_mutex> lock(frameMutex);
    
    if (index >= framePaths.size()) {
        std::cerr << "Animation::ensureFrameLoaded: Invalid frame index: " << index << std::endl;
        return nullptr;
    }
    
    // First, check if we already have this frame
    for (const auto& pair : loadedFrames) {
        if (pair.first == index) {
            return pair.second;
        }
    }
    
    try {
        // Create new texture
        auto texture = std::make_shared<sf::Texture>();
        if (!texture) {
            std::cerr << "Animation::ensureFrameLoaded: Failed to allocate texture" << std::endl;
            return nullptr;
        }
        
        texture->setSmooth(true);
        
        if (!texture->loadFromFile(framePaths[index].string())) {
            std::cerr << "Animation::ensureFrameLoaded: Failed to load texture: " << framePaths[index] << std::endl;
            return nullptr;
        }
        
        // Calculate memory usage
        auto size = texture->getSize();
        size_t frameMemory = size.x * size.y * 4;
        
        // Add to loaded frames
        loadedFrames.push_back({index, texture});
        totalMemoryUsage += frameMemory;
        
        // If this is the first frame ever loaded, set up the sprite
        if (loadedFrames.size() == 1 && !currentTexture) {
            currentTexture = texture;
            currentSprite.setTexture(*texture, true);
            auto bounds = currentSprite.getLocalBounds();
            currentSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        }
        
        // Only maintain frame window if we're not in initial loading
        if (!isInitialLoad) {
            maintainFrameWindow();
        }
        
        return texture;
    } catch (const std::exception& e) {
        std::cerr << "Animation::ensureFrameLoaded: Exception: " << e.what() << std::endl;
        return nullptr;
    }
}

bool Animation::loadFrame(size_t index) {
    std::lock_guard<std::recursive_mutex> lock(frameMutex);
    
    if (index >= framePaths.size()) {
        std::cerr << "Animation::loadFrame: Invalid frame index: " << index << std::endl;
        return false;
    }
    
    // Simply delegate to ensureFrameLoaded
    return ensureFrameLoaded(index) != nullptr;
}

void Animation::maintainFrameWindow() {
    // Calculate the window of frames we want to keep
    size_t halfWindow = maxLoadedFrames / 2;
    size_t windowStart = (currentFrame >= halfWindow) ? currentFrame - halfWindow : 0;
    size_t windowEnd = std::min(windowStart + maxLoadedFrames, framePaths.size());
    
    // Create a new deque for the frames we want to keep
    std::deque<std::pair<size_t, std::shared_ptr<sf::Texture>>> newFrames;
    
    // First, keep the current texture if it exists
    if (currentTexture) {
        for (const auto& pair : loadedFrames) {
            if (pair.second == currentTexture) {
                newFrames.push_back(pair);
                break;
            }
        }
    }
    
    // Then add all frames within our window
    for (const auto& pair : loadedFrames) {
        if (pair.first >= windowStart && pair.first <= windowEnd) {
            // Don't add the current texture again
            if (!currentTexture || pair.second != currentTexture) {
                newFrames.push_back(pair);
            }
        } else {
            // Calculate memory to free
            auto size = pair.second->getSize();
            size_t frameMemory = size.x * size.y * 4;
            totalMemoryUsage -= frameMemory;
        }
    }
    
    // If we still have too many frames, remove the ones furthest from current frame
    while (newFrames.size() > maxLoadedFrames) {
        // Find the frame furthest from current frame
        auto furthestIt = newFrames.begin();
        size_t maxDistance = 0;
        
        for (auto it = newFrames.begin(); it != newFrames.end(); ++it) {
            if (it->second == currentTexture) continue;
            
            size_t distance = std::abs(static_cast<ptrdiff_t>(it->first) - static_cast<ptrdiff_t>(currentFrame));
            if (distance > maxDistance) {
                maxDistance = distance;
                furthestIt = it;
            }
        }
        
        // If we couldn't find a frame to remove, break
        if (maxDistance == 0) break;
        
        // Remove the frame
        auto size = furthestIt->second->getSize();
        size_t frameMemory = size.x * size.y * 4;
        totalMemoryUsage -= frameMemory;
        newFrames.erase(furthestIt);
    }
    
    // Finally, swap in our new frame collection
    loadedFrames.swap(newFrames);
}

void Animation::update(float deltaTime) {
    if (!playing || framePaths.empty()) {
        return;
    }
    
    currentTime += deltaTime;
    
    // Cap deltaTime to prevent huge frame jumps
    float maxDeltaTime = frameTime * 5.0f;  // Max 5 frame jump
    if (deltaTime > maxDeltaTime) {
        deltaTime = maxDeltaTime;
        currentTime = currentFrame * frameTime + deltaTime;
    }
    
    size_t newFrame = static_cast<size_t>(currentTime / frameTime);
    
    if (newFrame >= framePaths.size()) {
        if (isLooping) {
            currentTime = 0.0f;
            newFrame = 0;
        } else {
            playing = false;
            newFrame = framePaths.size() - 1;
            return;
        }
    }
    
    if (newFrame != currentFrame) {
        currentFrame = newFrame;
        auto texture = ensureFrameLoaded(currentFrame);
        if (texture) {
            currentTexture = texture;
            currentSprite.setTexture(*currentTexture, true);
        } else {
            std::cerr << "Animation::update - Failed to load frame " << currentFrame << std::endl;
        }
    }
}

sf::Sprite& Animation::getCurrentFrame() {
    if (!currentTexture) {
        // Try to recover by loading current frame
        auto texture = ensureFrameLoaded(currentFrame);
        if (texture) {
            currentTexture = texture;
            currentSprite.setTexture(*currentTexture, true);
        }
    }
    
    return currentSprite;
}

void Animation::play() {
    if (framePaths.empty()) {
        std::cerr << "Animation::play: Cannot play animation with no frames" << std::endl;
        return;
    }
    
    // Ensure first frame is loaded
    if (!ensureFrameLoaded(currentFrame)) {
        std::cerr << "Animation::play: Failed to load initial frame" << std::endl;
        return;
    }
    
    playing = true;
}

void Animation::pause() {
    playing = false;
}

void Animation::stop() {
    playing = false;
    currentFrame = 0;
    currentTime = 0;
}

void Animation::reset() {
    currentFrame = 0;
    currentTime = 0;
} 