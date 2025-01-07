#pragma once

#include <SFML/Graphics.hpp>

namespace Engine {

enum class Anchor {
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

class ScalingManager {
public:
    static constexpr unsigned int BASE_WIDTH = 1280;
    static constexpr unsigned int BASE_HEIGHT = 720;

    static ScalingManager& getInstance();

    void updateWindowSize(unsigned int width, unsigned int height);
    sf::Vector2f convertNormalizedToScreen(float x, float y, Anchor anchor = Anchor::TopLeft) const;
    sf::Vector2f convertScreenToNormalized(float x, float y) const;
    sf::Vector2f getScaleFactors() const;
    float getScaledFontSize(float baseSize) const;

    // Utility functions for converting between absolute and normalized coordinates
    static sf::Vector2f absoluteToNormalized(float x, float y) {
        return sf::Vector2f(x / BASE_WIDTH, y / BASE_HEIGHT);
    }
    
    static sf::Vector2f absoluteToNormalized(const sf::Vector2f& absolute) {
        return absoluteToNormalized(absolute.x, absolute.y);
    }
    
    static sf::Vector2f normalizedToAbsolute(float x, float y) {
        return sf::Vector2f(x * BASE_WIDTH, y * BASE_HEIGHT);
    }
    
    static sf::Vector2f normalizedToAbsolute(const sf::Vector2f& normalized) {
        return normalizedToAbsolute(normalized.x, normalized.y);
    }

    // New sprite scaling methods
    void scaleSprite(sf::Sprite& sprite, const sf::Vector2f& normalizedPosition, Anchor anchor = Anchor::TopLeft) const;
    void scaleSpriteToFill(sf::Sprite& sprite) const;
    void scaleSpriteWithAspectRatio(sf::Sprite& sprite, bool fillScreen = false) const;

    // Delete copy constructor and assignment operator
    ScalingManager(const ScalingManager&) = delete;
    ScalingManager& operator=(const ScalingManager&) = delete;

private:
    ScalingManager(); // Private constructor for singleton
    
    unsigned int currentWidth;
    unsigned int currentHeight;
    float scaleX;
    float scaleY;

    void updateScaleFactors();
    sf::Vector2f getAnchorOffset(Anchor anchor) const;
};

} // namespace Engine 