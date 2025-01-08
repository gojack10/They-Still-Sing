#include "WarningState.hpp"
#include "../core/StateManager.hpp"
#include "MainMenuState.hpp"
#include "../systems/ui/ScalingManager.hpp"
#include "../config/AssetPaths.hpp"
#include <iostream>
#include <cmath>

WarningState::WarningState() : startFade(false), hasTransitioned(false), opacity(255), fadeTime(5.0f) {
    init();
}

void WarningState::init() {
    if (!warningTexture.loadFromFile(AssetPaths::WARNING_TEXTURE)) {
        throw std::runtime_error("Failed to load warning texture");
    }
    warningSprite.setTexture(warningTexture);
    warningSprite.setOrigin(
        warningTexture.getSize().x / 2.f,
        warningTexture.getSize().y / 2.f
    );
    timer.restart();
}

void WarningState::cleanup() {
}

void WarningState::handleInput(sf::RenderWindow& window) {
    if (!startFade && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
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
            StateManager::getInstance().changeState(std::make_unique<MainMenuState>());
            return;  // Exit immediately after state change
        }
        
        sf::Color color = warningSprite.getColor();
        color.a = static_cast<sf::Uint8>(std::round(opacity));
        warningSprite.setColor(color);
    }
}

void WarningState::draw(sf::RenderWindow& window) {
    // Update ScalingManager with current window size
    Engine::ScalingManager::getInstance().updateWindowSize(window.getSize().x, window.getSize().y);
    
    // Scale the warning sprite to fill the screen
    Engine::ScalingManager::getInstance().scaleSpriteToFill(warningSprite);
    
    window.draw(warningSprite);
} 