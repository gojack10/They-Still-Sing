#include "MainMenuState.hpp"
#include "../config/AssetPaths.hpp"
#include "../systems/animation/AnimationManager.hpp"
#include "../systems/ui/ScalingManager.hpp"
#include "../ui/MenuManager.hpp"
#include "../systems/audio_systems/AudioSystem.hpp"
#include "../core/StateManager.hpp"
#include "OptionsState.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

MainMenuState::MainMenuState() : lastHoveredButton("") {
    std::cout << "MainMenuState: Constructor called" << std::endl;
    
    // Verify audio system initialization
    auto& audio = Engine::AudioSystem::getInstance();
    try {
        if (audio.getSoundStatus("menu-hover") == sf::SoundSource::Stopped) {
            std::cout << "MainMenuState: Audio system initialized and menu-hover sound is loaded" << std::endl;
        } else {
            std::cout << "MainMenuState: Warning - Unexpected menu-hover sound status" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "MainMenuState: Error checking audio system - " << e.what() << std::endl;
    }
    
    init();
    // Initialize menu hitboxes and load menu text placement
    MenuManager::getInstance().loadFromJson(AssetPaths::MENU_CONFIG);
    loadMenuTextPlacement();
}

void MainMenuState::loadMenuTextPlacement() {
    // Load menu text placement from config
    std::ifstream file(AssetPaths::MENU_CONFIG);
    if (!file.is_open()) {
        std::cerr << "MainMenuState: Failed to open menu configuration file!" << std::endl;
        return;
    }

    nlohmann::json j;
    file >> j;

    // Get the menu text placement
    const auto& placement = j["button_placement"][0];
    menuTextPlacement.position = sf::Vector2f(placement["x"].get<float>(), placement["y"].get<float>());
    menuTextPlacement.normalizedPosition = Engine::ScalingManager::absoluteToNormalized(menuTextPlacement.position);
    
    // Setup the sprite
    menuTextSprite.setTexture(menuTextTexture);
}

void MainMenuState::init() {
    std::cout << "MainMenuState: Initializing..." << std::endl;
    
    // Set current state in MenuManager
    MenuManager::getInstance().setCurrentState("MainMenu");
    
    // Load menu text texture
    if (!menuTextTexture.loadFromFile(AssetPaths::MENU_TEXT_TEXTURE)) {
        std::cerr << "MainMenuState: Failed to load menu text texture!" << std::endl;
        return;
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
                
                anim->setMaxLoadedFrames(60);  // 2 seconds at 30 FPS
                anim->setFrameTime(1.0f/30.0f);  // 30 FPS
                anim->setLooping(true);
                
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

void MainMenuState::cleanup() {
}

void MainMenuState::handleInput(sf::RenderWindow& window) {
    MenuManager::getInstance().handleInput(window);
    
    // Get currently hovered button
    const std::string& currentHoveredButton = MenuManager::getInstance().getHoveredButton();
    
    // Handle button clicks
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (currentHoveredButton == "OPTIONS") {
            // Save current music state and transition to options
            auto& audio = Engine::AudioSystem::getInstance();
            StateManager::getInstance().changeState(std::make_unique<OptionsState>());
            return;
        }
    }
    
    // Play sound when entering a new button (not when leaving one)
    if (currentHoveredButton != lastHoveredButton && !currentHoveredButton.empty()) {
        Engine::AudioSystem::getInstance().playSound("menu-hover");
    }
    
    // Update last hovered button
    lastHoveredButton = currentHoveredButton;
}

void MainMenuState::update(float deltaTime) {
    try {
        static float timeAccumulator = 0.0f;
        timeAccumulator += deltaTime;
        
        float cappedDeltaTime = std::min(deltaTime, 0.1f);
        
        AnimationManager::getInstance().update(cappedDeltaTime);
        
        timeAccumulator = 0.0f;
    } catch (const std::exception& e) {
        std::cerr << "MainMenuState: Error during update: " << e.what() << std::endl;
    }
}

void MainMenuState::draw(sf::RenderWindow& window) {
    try {
        window.clear(sf::Color::Black);
        
        if (auto* anim = AnimationManager::getInstance().getAnimation("main_menu")) {
            if (!anim->hasFrames()) {
                std::cerr << "MainMenuState::draw: Animation has no frames!" << std::endl;
                return;
            }
            
            sf::Sprite& sprite = anim->getCurrentFrame();
            
            Engine::ScalingManager::getInstance().scaleSpriteToFill(sprite);
            
            window.draw(sprite);
            
            // Draw menu text at its configured position
            auto& scalingManager = Engine::ScalingManager::getInstance();
            scalingManager.scaleSprite(menuTextSprite, menuTextPlacement.normalizedPosition);
            window.draw(menuTextSprite);
        }
        
        // Draw menu hitboxes and selector
        MenuManager::getInstance().draw(window);
    } catch (const std::exception& e) {
        std::cerr << "MainMenuState: Error during draw: " << e.what() << std::endl;
    }
} 