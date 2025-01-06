#include "WarningState.hpp"
#include "MainMenuState.hpp"
#include "../core/StateManager.hpp"
#include "../config/AssetPaths.hpp"
#include <cmath>
#include <iostream>
#include <memory>

WarningState::WarningState() {
    std::cout << "WarningState: Initializing..." << std::endl;
    init();
}

void WarningState::init() {
    if (!warningTexture.loadFromFile(AssetPaths::WARNING_TEXTURE)) {
        std::cerr << "WarningState: Failed to load warning texture from: " 
                  << AssetPaths::WARNING_TEXTURE << std::endl;
        throw std::runtime_error("Failed to load warning texture");
    }
    std::cout << "WarningState: Successfully loaded warning texture" << std::endl;
    
    warningSprite.setTexture(warningTexture);
    
    // Center the sprite
    sf::Vector2u textureSize = warningTexture.getSize();
    warningSprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
    warningSprite.setPosition(1280 / 2.0f, 720 / 2.0f);
    
    // Scale to fit 1280x720 if needed
    float scaleX = 1280.0f / textureSize.x;
    float scaleY = 720.0f / textureSize.y;
    warningSprite.setScale(scaleX, scaleY);
    
    timer.restart();
    opacity = 255.0f;
    std::cout << "WarningState: Initialization complete" << std::endl;
}

void WarningState::handleInput(sf::RenderWindow& window) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {
        startFade = true;
        std::cout << "WarningState: User input received, starting fade" << std::endl;
    }
}

void WarningState::update(float deltaTime) {
    // Check if it's time to start fading
    if (!startFade && timer.getElapsedTime().asSeconds() >= fadeTime) {
        startFade = true;
        std::cout << "WarningState: Auto-fade starting after " << fadeTime << " seconds" << std::endl;
    }
    
    if (startFade && !hasTransitioned) {
        // Simple linear fade over 2 seconds
        opacity = std::max(0.0f, opacity - (128.0f * deltaTime));
        
        if (opacity <= 0) {
            opacity = 0;
            hasTransitioned = true;
            std::cout << "WarningState: Fade complete, transitioning to MainMenuState" << std::endl;
            StateManager::getInstance().changeState(std::make_unique<MainMenuState>());
            return;  // Exit immediately after state change
        }
        
        sf::Color color = warningSprite.getColor();
        color.a = static_cast<sf::Uint8>(std::round(opacity));
        warningSprite.setColor(color);
    }
}

void WarningState::draw(sf::RenderWindow& window) {
    window.draw(warningSprite);
} 