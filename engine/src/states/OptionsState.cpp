#include "OptionsState.hpp"
#include "../core/StateManager.hpp"
#include "MainMenuState.hpp"
#include "../systems/animation/AnimationManager.hpp"
#include "../systems/ui/ScalingManager.hpp"
#include "../config/AssetPaths.hpp"
#include "../ui/MenuManager.hpp"
#include <iostream>
#include <cmath>
#include <fstream>
#include <nlohmann/json.hpp>

OptionsState::OptionsState() 
    : backgroundColor(sf::Color(8, 16, 123)) // #08107b
    , isTransitioningIn(true)
    , isTransitioningOut(false)
    , transitionTime(0.0f)
    , transitionDuration(1.0f)
    , currentAnimation("options_enter")
    , animationComplete(false) {
    init();
}

void OptionsState::init() {
    std::cout << "OptionsState: Initializing..." << std::endl;
    
    auto& animManager = AnimationManager::getInstance();
    
    try {
        // Set current state in MenuManager
        MenuManager::getInstance().setCurrentState("Options");
        
        // Load the options-enter animation
        if (animManager.loadAnimation("options_enter", AssetPaths::OPTIONS_ENTER_ANIM, false)) {
            std::cout << "OptionsState: Successfully loaded options enter animation" << std::endl;
            
            if (auto* anim = animManager.getAnimation("options_enter")) {
                anim->setMaxLoadedFrames(60);  // 2 seconds at 30 FPS
                anim->setFrameTime(1.0f/30.0f);  // 30 FPS
                anim->setLooping(false);
                
                if (!anim->loadFrame(0)) {
                    std::cerr << "OptionsState: Failed to load first frame!" << std::endl;
                    return;
                }
                
                anim->play();
            }
        } else {
            std::cerr << "OptionsState: Failed to load options enter animation!" << std::endl;
        }
        
        // Also load the exit animation but don't play it yet
        if (animManager.loadAnimation("options_exit", AssetPaths::OPTIONS_EXIT_ANIM, false)) {
            std::cout << "OptionsState: Successfully loaded options exit animation" << std::endl;
        } else {
            std::cerr << "OptionsState: Failed to load options exit animation!" << std::endl;
        }

        // Load UI textures
        if (!optionsButtonsTexture.loadFromFile(AssetPaths::OPTIONS_BUTTONS_TEXTURE)) {
            std::cerr << "OptionsState: Failed to load options buttons texture!" << std::endl;
        }
        optionsButtonsSprite.setTexture(optionsButtonsTexture);

        if (!resetGameButtonTexture.loadFromFile(AssetPaths::RESET_ALL_PROGRESS_TEXTURE)) {
            std::cerr << "OptionsState: Failed to load reset game button texture!" << std::endl;
        }
        resetGameButtonSprite.setTexture(resetGameButtonTexture);

        if (!checkTexture.loadFromFile(AssetPaths::UI_CHECK)) {
            std::cerr << "OptionsState: Failed to load check texture!" << std::endl;
        }
        checkSprite.setTexture(checkTexture);

        // Load menu config for UI element placement
        std::ifstream configFile(AssetPaths::MENU_CONFIG);
        if (!configFile.is_open()) {
            throw std::runtime_error("Failed to open menu config file");
        }
        nlohmann::json config = nlohmann::json::parse(configFile);

        // Position UI elements according to menu config
        auto& placements = config["button_placement"];
        for (const auto& placement : placements) {
            std::string name = placement["name"];
            float x = placement["x"];
            float y = placement["y"];

            if (name == "OPTIONS_MENU_BUTTONS") {
                optionsButtonsSprite.setPosition(x, y);
            } else if (name == "RESET_ALL_PROGRESS") {
                resetGameButtonSprite.setPosition(x, y);
            } else if (name == "FULLSCREEN_CHECK") {
                checkSprite.setPosition(x, y);
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "OptionsState: Error during initialization: " << e.what() << std::endl;
    }
}

void OptionsState::cleanup() {
    // Stop any ongoing animations
    if (auto* anim = AnimationManager::getInstance().getAnimation(currentAnimation)) {
        anim->stop();
    }
    
    // Clear all sprites and hitboxes
    optionsButtonsSprite = sf::Sprite();
    resetGameButtonSprite = sf::Sprite();
    checkSprite = sf::Sprite();
    MenuManager::getInstance().clearHitboxes();
}

void OptionsState::handleInput(sf::RenderWindow& window) {
    if (!isTransitioningOut && animationComplete) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) || 
            (sf::Mouse::isButtonPressed(sf::Mouse::Left) && 
             MenuManager::getInstance().isHitboxClicked("BACK_TO_MAIN_MENU_BUTTON", window))) {
            
            isTransitioningOut = true;
            currentAnimation = "options_exit";
            if (auto* anim = AnimationManager::getInstance().getAnimation("options_exit")) {
                anim->reset();
                anim->play();
            }
            
            // Clear all sprites and hitboxes
            optionsButtonsSprite = sf::Sprite();
            resetGameButtonSprite = sf::Sprite();
            checkSprite = sf::Sprite();
            MenuManager::getInstance().clearHitboxes();
        }
    }
}

void OptionsState::update(float deltaTime) {
    try {
        auto& animManager = AnimationManager::getInstance();
        
        // Update current animation
        animManager.update(deltaTime);
        
        // Handle animation state
        if (auto* anim = animManager.getAnimation(currentAnimation)) {
            if (!anim->isPlaying() && !animationComplete && currentAnimation == "options_enter") {
                animationComplete = true;
            }
            else if (!anim->isPlaying() && isTransitioningOut && currentAnimation == "options_exit") {
                // Transition back to main menu
                StateManager::getInstance().changeState(std::make_unique<MainMenuState>());
                return;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "OptionsState: Error during update: " << e.what() << std::endl;
    }
}

void OptionsState::draw(sf::RenderWindow& window) {
    try {
        // Clear with the options menu background color
        window.clear(backgroundColor);
        
        // Only draw animation during transitions (not complete) or when transitioning out
        if (!animationComplete || isTransitioningOut) {
            if (auto* anim = AnimationManager::getInstance().getAnimation(currentAnimation)) {
                if (anim->hasFrames()) {
                    sf::Sprite& sprite = anim->getCurrentFrame();
                    Engine::ScalingManager::getInstance().scaleSpriteToFill(sprite);
                    window.draw(sprite);
                }
            }
        } else {
            // Draw UI elements when not transitioning
            window.draw(optionsButtonsSprite);
            window.draw(resetGameButtonSprite);
            window.draw(checkSprite);

            // Draw hitboxes through MenuManager
            MenuManager::getInstance().draw(window);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "OptionsState: Error during draw: " << e.what() << std::endl;
    }
} 