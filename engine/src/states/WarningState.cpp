#include "WarningState.hpp"
#include "../core/StateManager.hpp"
#include "../config/AssetPaths.hpp"
#include <cmath>

WarningState::WarningState() {
    init();
}

void WarningState::init() {
    if (!warningTexture.loadFromFile(AssetPaths::UI_TEXTURES + "/warning.jpg")) {
        // Handle error
        return;
    }
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
}

void WarningState::handleInput(sf::RenderWindow& window) {
    // Don't poll events here since main loop handles it
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {
        startFade = true;
    }
}

void WarningState::update(float deltaTime) {
    // Check if it's time to start fading
    if (!startFade && timer.getElapsedTime().asSeconds() >= fadeTime) {
        startFade = true;
    }
    
    if (startFade) {
        // Simple linear fade over 2 seconds
        opacity = std::max(0.0f, opacity - (128.0f * deltaTime));
        
        if (opacity <= 0) {
            opacity = 0;
            // TODO: Transition to main menu state
            // StateManager::getInstance().changeState(new MainMenuState());
        }
        
        sf::Color color = warningSprite.getColor();
        color.a = static_cast<sf::Uint8>(std::round(opacity));
        warningSprite.setColor(color);
    }
}

void WarningState::draw(sf::RenderWindow& window) {
    window.draw(warningSprite);
} 