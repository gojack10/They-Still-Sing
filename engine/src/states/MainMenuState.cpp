#include "MainMenuState.hpp"
#include "../config/AssetPaths.hpp"
#include "../systems/animation/AnimationManager.hpp"
#include "../utils/Debug.hpp"
#include "../systems/ui/ScalingManager.hpp"
#include "../ui/MenuManager.hpp"
#include <iostream>
#include <filesystem>

MainMenuState::MainMenuState() {
    init();
    // Initialize menu hitboxes
    MenuManager::getInstance().loadFromJson(AssetPaths::MENU_CONFIG);
}

void MainMenuState::init() {
    std::cout << "MainMenuState: Initializing..." << std::endl;
    
    // Load menu text texture
    if (!menuTextTexture.loadFromFile(AssetPaths::MENU_TEXT_TEXTURE)) {
        std::cerr << "MainMenuState: Failed to load menu text texture!" << std::endl;
    } else {
        menuTextSprite.setTexture(menuTextTexture);
        // Store normalized position (0-1 range)
        menuTextBasePosition = sf::Vector2f(0.0625f, 0.0944f); // 80/1280, 68/720
    }
    
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
                
                anim->play();
            }
        } else {
            std::cerr << "MainMenuState: Failed to load main menu animation!" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "MainMenuState: Error during initialization: " << e.what() << std::endl;
    }
}

void MainMenuState::handleInput(sf::RenderWindow& window) {
    MenuManager::getInstance().handleInput(window);
    
    // Check if NEW_GAME is selected and clicked
    if (MenuManager::getInstance().getHoveredButton() == "NEW_GAME" && 
        sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        // Handle new game action
    }
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
        
        timeAccumulator = 0.0f;
    } catch (const std::exception& e) {
        std::cerr << "MainMenuState: Error during update: " << e.what() << std::endl;
    }
}

void MainMenuState::draw(sf::RenderWindow& window) {
    try {
        DEBUG_LOCATION("MainMenuState::draw - Start");
        window.clear(sf::Color::Black);
        
        if (auto* anim = AnimationManager::getInstance().getAnimation("main_menu")) {
            DEBUG_LOCATION("MainMenuState::draw - Got animation");
            if (!anim->hasFrames()) {
                std::cerr << "MainMenuState::draw: Animation has no frames!" << std::endl;
                return;
            }
            
            // Get the sprite and scale it
            DEBUG_LOCATION("MainMenuState::draw - Getting current frame");
            sf::Sprite& sprite = anim->getCurrentFrame();
            
            // Scale the sprite to fill the screen
            Engine::ScalingManager::getInstance().scaleSpriteToFill(sprite);
            
            // Draw the sprite
            DEBUG_LOCATION("MainMenuState::draw - Drawing sprite");
            window.draw(sprite);
            
            // Scale and draw the menu text using normalized coordinates
            Engine::ScalingManager::getInstance().scaleSprite(menuTextSprite, menuTextBasePosition);
            window.draw(menuTextSprite);
        }
        
        // Draw menu hitboxes and selector
        MenuManager::getInstance().draw(window);
    } catch (const std::exception& e) {
        std::cerr << "MainMenuState: Error during draw: " << e.what() << std::endl;
    }
} 