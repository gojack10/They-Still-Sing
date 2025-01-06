#include "Animation.hpp"
#include "../../utils/Debug.hpp"
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
    DEBUG_LOCATION("Animation::Constructor");
    std::cout << "Animation: Constructor called" << std::endl;
    currentSprite.setPosition(0, 0);
}

Animation::~Animation() {
    DEBUG_LOCATION("Animation::Destructor");
    std::cout << "Animation: Destructor called, cleaning up " << loadedFrames.size() << " frames" << std::endl;
    std::lock_guard<std::recursive_mutex> lock(frameMutex);
    loadedFrames.clear();
    framePaths.clear();
    currentTexture.reset();
    std::cout << "Animation: All frames cleaned up" << std::endl;
}

bool Animation::loadFromDirectory(const std::string& path, const std::string& extension) {
    DEBUG_LOCATION("Animation::loadFromDirectory - Start");
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
            DEBUG_LOCATION("Animation::loadFromDirectory - Clearing frames");
            std::lock_guard<std::recursive_mutex> lock(frameMutex);
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
            DEBUG_LOCATION("Animation::loadFromDirectory - Storing paths");
            std::lock_guard<std::recursive_mutex> lock(frameMutex);
            framePaths = std::move(files);
        }
        
        std::cout << "Animation::loadFromDirectory: Found " << framePaths.size() << " frames" << std::endl;
        
        // Load initial frame to get dimensions
        DEBUG_LOCATION("Animation::loadFromDirectory - Loading first frame");
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
    DEBUG_LOCATION("Animation::ensureFrameLoaded - Start");
    std::lock_guard<std::recursive_mutex> lock(frameMutex);
    
    if (index >= framePaths.size()) {
        std::cerr << "Animation::ensureFrameLoaded: Invalid frame index: " << index << std::endl;
        return nullptr;
    }
    
    // Track current state
    DEBUG_MEMORY_WRITE(&currentFrame);
    DEBUG_MEMORY_READ(&loadedFrames);
    
    // First, check if we already have this frame
    for (const auto& pair : loadedFrames) {
        if (pair.first == index) {
            DEBUG_MEMORY_READ(pair.second.get());
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
        
        DEBUG_LOCATION("Animation::ensureFrameLoaded - Loading texture from file: " + framePaths[index].string());
        if (!texture->loadFromFile(framePaths[index].string())) {
            std::cerr << "Animation::ensureFrameLoaded: Failed to load texture: " << framePaths[index] << std::endl;
            return nullptr;
        }
        
        // Calculate memory usage
        auto size = texture->getSize();
        size_t frameMemory = size.x * size.y * 4;
        
        // Track texture memory
        DEBUG_MEMORY_WRITE(texture.get());
        
        // Add to loaded frames
        loadedFrames.push_back({index, texture});
        totalMemoryUsage += frameMemory;
        
        // If this is the first frame ever loaded, set up the sprite
        if (loadedFrames.size() == 1 && !currentTexture) {
            DEBUG_LOCATION("Animation::ensureFrameLoaded - Setting up first frame");
            DEBUG_MEMORY_WRITE(&currentTexture);
            currentTexture = texture;
            DEBUG_MEMORY_WRITE(&currentSprite);
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
    DEBUG_LOCATION("Animation::loadFrame - Start");
    std::lock_guard<std::recursive_mutex> lock(frameMutex);
    
    if (index >= framePaths.size()) {
        std::cerr << "Animation::loadFrame: Invalid frame index: " << index << std::endl;
        return false;
    }
    
    // Simply delegate to ensureFrameLoaded
    return ensureFrameLoaded(index) != nullptr;
}

void Animation::maintainFrameWindow() {
    DEBUG_LOCATION("Animation::maintainFrameWindow");
    
    DEBUG_MEMORY_READ(&currentFrame);
    DEBUG_MEMORY_READ(&maxLoadedFrames);
    DEBUG_MEMORY_READ(&framePaths);
    DEBUG_MEMORY_READ(&loadedFrames);
    DEBUG_MEMORY_READ(&currentTexture);
    
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
    DEBUG_MEMORY_WRITE(&loadedFrames);
    loadedFrames.swap(newFrames);
}

void Animation::update(float deltaTime) {
    DEBUG_LOCATION("Animation::update - Start");
    std::lock_guard<std::recursive_mutex> lock(frameMutex);
    
    if (!playing || framePaths.empty()) {
        return;
    }
    
    std::cout << "Animation::update - deltaTime: " << deltaTime 
              << ", currentFrame: " << currentFrame 
              << ", loadedFrames: " << loadedFrames.size() << std::endl;
    
    currentTime += deltaTime;
    
    // Cap deltaTime to prevent huge frame jumps
    float maxDeltaTime = frameTime * 5.0f;  // Max 5 frame jump
    if (deltaTime > maxDeltaTime) {
        std::cout << "Animation::update - Capping large deltaTime from " 
                  << deltaTime << " to " << maxDeltaTime << std::endl;
        deltaTime = maxDeltaTime;
        currentTime = currentFrame * frameTime + deltaTime;
    }
    
    size_t newFrame = static_cast<size_t>(currentTime / frameTime);
    
    if (newFrame >= framePaths.size()) {
        if (isLooping) {
            currentTime = 0.0f;
            newFrame = 0;
            std::cout << "Animation::update - Looping back to start" << std::endl;
        } else {
            playing = false;
            newFrame = framePaths.size() - 1;
            std::cout << "Animation::update - Reached end of animation" << std::endl;
            return;
        }
    }
    
    if (newFrame != currentFrame) {
        std::cout << "Animation::update - Advancing from frame " << currentFrame 
                  << " to " << newFrame << std::endl;
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
    DEBUG_LOCATION("Animation::getCurrentFrame");
    std::lock_guard<std::recursive_mutex> lock(frameMutex);
    
    std::cout << "Animation::getCurrentFrame - Frame " << currentFrame 
              << ", hasTexture: " << (currentTexture != nullptr) << std::endl;
    
    if (!currentTexture) {
        std::cerr << "Animation::getCurrentFrame - No texture loaded!" << std::endl;
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
    DEBUG_LOCATION("Animation::play");
    std::lock_guard<std::recursive_mutex> lock(frameMutex);
    
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
    DEBUG_LOCATION("Animation::pause");
    std::lock_guard<std::recursive_mutex> lock(frameMutex);
    playing = false;
}

void Animation::stop() {
    DEBUG_LOCATION("Animation::stop");
    std::lock_guard<std::recursive_mutex> lock(frameMutex);
    playing = false;
    currentFrame = 0;
    currentTime = 0;
}

void Animation::reset() {
    DEBUG_LOCATION("Animation::reset");
    std::lock_guard<std::recursive_mutex> lock(frameMutex);
    currentFrame = 0;
    currentTime = 0;
} 