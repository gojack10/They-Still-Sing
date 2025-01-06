#include "MainMenuState.hpp"
#include "../config/AssetPaths.hpp"
#include "../systems/animation/AnimationManager.hpp"
#include "../utils/Debug.hpp"
#include <iostream>
#include <filesystem>

MainMenuState::MainMenuState() {
    init();
}

void MainMenuState::init() {
    std::cout << "MainMenuState: Initializing..." << std::endl;
    
    auto& animManager = AnimationManager::getInstance();
    
    try {
        if (!std::filesystem::exists(AssetPaths::MAIN_MENU_ANIM)) {
            std::cerr << "MainMenuState: Animation directory does not exist: " << AssetPaths::MAIN_MENU_ANIM << std::endl;
            return;
        }
        
        // Reset any existing animation first
        if (auto* existingAnim = animManager.getAnimation("main_menu")) {
            existingAnim->stop();
        }
        
        if (animManager.loadAnimation("main_menu", AssetPaths::MAIN_MENU_ANIM)) {
            std::cout << "MainMenuState: Successfully loaded main menu animation from: " 
                      << AssetPaths::MAIN_MENU_ANIM << std::endl;
            
            if (auto* anim = animManager.getAnimation("main_menu")) {
                if (!anim->hasFrames()) {
                    std::cerr << "MainMenuState: Animation loaded but contains no frames!" << std::endl;
                    return;
                }
                
                // Configure memory management - keep 2 seconds worth of frames in memory
                anim->setMaxLoadedFrames(60);  // 2 seconds at 30 FPS
                
                // Set animation properties
                anim->setFrameTime(1.0f/30.0f);  // 30 FPS
                anim->setLooping(true);
                
                // Ensure first frame is loaded before starting
                if (!anim->loadFrame(0)) {
                    std::cerr << "MainMenuState: Failed to load first frame!" << std::endl;
                    return;
                }
                
                std::cout << "MainMenuState: Animation configured with " << anim->getFrameCount() 
                          << " frames at 30 FPS" << std::endl;
                std::cout << "MainMenuState: Using frame streaming with 60 frame window" << std::endl;
                
                anim->play();
                std::cout << "MainMenuState: Started playing animation" << std::endl;
            } else {
                std::cerr << "MainMenuState: Failed to get animation after loading!" << std::endl;
            }
        } else {
            std::cerr << "MainMenuState: Failed to load main menu animation from: " 
                      << AssetPaths::MAIN_MENU_ANIM << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "MainMenuState: Error during initialization: " << e.what() << std::endl;
    }
}

void MainMenuState::handleInput(sf::RenderWindow& window) {
    // Handle menu input here
}

void MainMenuState::update(float deltaTime) {
    try {
        DEBUG_LOCATION("MainMenuState::update - Start");
        static float timeAccumulator = 0.0f;
        timeAccumulator += deltaTime;
        
        // Cap deltaTime for animation update to prevent huge jumps
        float cappedDeltaTime = std::min(deltaTime, 0.1f);  // Cap at 100ms
        
        // Update animation
        DEBUG_LOCATION("MainMenuState::update - Updating animation");
        AnimationManager::getInstance().update(cappedDeltaTime);
        
        // Print debug info every second
        if (timeAccumulator >= 1.0f) {
            if (auto* anim = AnimationManager::getInstance().getAnimation("main_menu")) {
                std::cout << "MainMenuState: Animation is " 
                          << (anim->isPlaying() ? "playing" : "not playing") 
                          << " (frame count: " << anim->getFrameCount() << ")" << std::endl;
            }
            timeAccumulator = 0.0f;
        }
    } catch (const std::exception& e) {
        std::cerr << "MainMenuState: Error during update: " << e.what() << std::endl;
    }
}

void MainMenuState::draw(sf::RenderWindow& window) {
    try {
        DEBUG_LOCATION("MainMenuState::draw - Start");
        // Clear the window with black color first
        window.clear(sf::Color::Black);
        
        if (auto* anim = AnimationManager::getInstance().getAnimation("main_menu")) {
            DEBUG_LOCATION("MainMenuState::draw - Got animation");
            if (!anim->hasFrames()) {
                std::cerr << "MainMenuState::draw: Animation has no frames!" << std::endl;
                return;
            }
            
            // Get the sprite
            DEBUG_LOCATION("MainMenuState::draw - Getting current frame");
            sf::Sprite& sprite = anim->getCurrentFrame();
            
            // Verify the sprite has a valid texture
            DEBUG_LOCATION("MainMenuState::draw - Verifying texture");
            const sf::Texture* texture = sprite.getTexture();
            if (!texture) {
                std::cerr << "MainMenuState::draw: Sprite has no texture!" << std::endl;
                return;
            }
            
            // Get current sprite bounds and window size
            DEBUG_LOCATION("MainMenuState::draw - Calculating scale");
            sf::Vector2u textureSize = texture->getSize();
            sf::Vector2u windowSize = window.getSize();
            
            // Calculate scale to fill window completely (no black borders)
            float scaleX = static_cast<float>(windowSize.x) / static_cast<float>(textureSize.x);
            float scaleY = static_cast<float>(windowSize.y) / static_cast<float>(textureSize.y);
            float scale = std::max(scaleX, scaleY); // Use max instead of min to ensure no black borders
            
            // Set sprite origin to center
            sprite.setOrigin(
                static_cast<float>(textureSize.x) / 2.f,
                static_cast<float>(textureSize.y) / 2.f
            );
            
            // Set sprite scale
            DEBUG_LOCATION("MainMenuState::draw - Setting sprite properties");
            sprite.setScale(scale, scale);
            
            // Center the sprite in the window
            sprite.setPosition(
                static_cast<float>(windowSize.x) / 2.f,
                static_cast<float>(windowSize.y) / 2.f
            );
            
            // Draw the sprite
            DEBUG_LOCATION("MainMenuState::draw - Drawing sprite");
            window.draw(sprite);
        }
    } catch (const std::exception& e) {
        std::cerr << "MainMenuState: Error during draw: " << e.what() << std::endl;
    }
} 