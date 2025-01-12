#include "OptionsState.hpp"
#include "../core/StateManager.hpp"
#include "MainMenuState.hpp"
#include "../systems/animation/AnimationManager.hpp"
#include "../systems/ui/ScalingManager.hpp"
#include "../config/AssetPaths.hpp"
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

        // Load menu config
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

        // Setup hitboxes
        auto& hitboxes = config["button_hitboxes"];
        for (const auto& hitbox : hitboxes) {
            std::string name = hitbox["name"];
            float x = hitbox["x"];
            float y = hitbox["y"];
            float w = hitbox["w"];
            float h = hitbox["h"];

            sf::RectangleShape* targetHitbox = nullptr;

            if (name == "FULLSCREEN_CHECKBOX") targetHitbox = &fullscreenHitbox;
            else if (name == "SFX_VOL_DOWN_BUTTON") targetHitbox = &sfxVolDownHitbox;
            else if (name == "SFX_VOL_UP_BUTTON") targetHitbox = &sfxVolUpHitbox;
            else if (name == "MUSIC_VOL_DOWN_BUTTON") targetHitbox = &musicVolDownHitbox;
            else if (name == "MUSIC_VOL_UP_BUTTON") targetHitbox = &musicVolUpHitbox;
            else if (name == "RESET_ALL_PROGRESS_BUTTON") targetHitbox = &resetAllHitbox;

            if (targetHitbox) {
                targetHitbox->setSize(sf::Vector2f(w, h));
                targetHitbox->setPosition(x, y);
                targetHitbox->setFillColor(sf::Color::Transparent);
                targetHitbox->setOutlineColor(sf::Color::Green);
                targetHitbox->setOutlineThickness(1.0f);
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
}

void OptionsState::handleInput(sf::RenderWindow& window) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && !isTransitioningOut && animationComplete) {
        isTransitioningOut = true;
        currentAnimation = "options_exit";
        if (auto* anim = AnimationManager::getInstance().getAnimation("options_exit")) {
            anim->reset();
            anim->play();
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

            // Draw hitboxes
            window.draw(fullscreenHitbox);
            window.draw(sfxVolDownHitbox);
            window.draw(sfxVolUpHitbox);
            window.draw(musicVolDownHitbox);
            window.draw(musicVolUpHitbox);
            window.draw(resetAllHitbox);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "OptionsState: Error during draw: " << e.what() << std::endl;
    }
} 