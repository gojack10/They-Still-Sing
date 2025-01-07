#pragma once

#include <SFML/Graphics.hpp>

class UIScaler {
public:
    // Base resolution that we're designing for
    static constexpr float BASE_WIDTH = 1280.0f;
    static constexpr float BASE_HEIGHT = 720.0f;

    static sf::Vector2f getScaleFactors(const sf::RenderWindow& window) {
        sf::Vector2u windowSize = window.getSize();
        return sf::Vector2f(
            windowSize.x / BASE_WIDTH,
            windowSize.y / BASE_HEIGHT
        );
    }

    static void scaleSprite(sf::Sprite& sprite, const sf::RenderWindow& window, 
                           const sf::Vector2f& basePosition) {
        sf::Vector2f scale = getScaleFactors(window);
        
        // Scale the sprite position based on window size
        sprite.setPosition(
            basePosition.x * scale.x,
            basePosition.y * scale.y
        );
        
        // Scale the sprite size while maintaining aspect ratio
        sprite.setScale(scale);
    }
}; 