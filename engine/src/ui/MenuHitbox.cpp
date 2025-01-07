#include "MenuHitbox.hpp"
#include "../systems/ui/ScalingManager.hpp"
#include <random>

MenuHitbox::MenuHitbox(const sf::Vector2f& absolutePosition, const sf::Vector2f& absoluteSize, const std::string& name)
    : normalizedPosition(Engine::ScalingManager::absoluteToNormalized(absolutePosition))
    , normalizedSize(Engine::ScalingManager::absoluteToNormalized(absoluteSize))
    , name(name) {
    // Generate random color for debug visualization
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    debugColor = sf::Color(dis(gen), dis(gen), dis(gen), 204); // 80% opacity (204/255)
}

bool MenuHitbox::contains(const sf::Vector2f& normalizedPoint) const {
    return normalizedPoint.x >= normalizedPosition.x &&
           normalizedPoint.x <= normalizedPosition.x + normalizedSize.x &&
           normalizedPoint.y >= normalizedPosition.y &&
           normalizedPoint.y <= normalizedPosition.y + normalizedSize.y;
}

void MenuHitbox::draw(sf::RenderWindow& window, bool debugMode) const {
    if (debugMode) {
        auto& scalingManager = Engine::ScalingManager::getInstance();
        
        // Convert normalized coordinates to screen coordinates
        sf::Vector2f screenPos = scalingManager.convertNormalizedToScreen(
            normalizedPosition.x, normalizedPosition.y);
        
        // Convert normalized size to screen size using scale factors
        sf::Vector2f scale = scalingManager.getScaleFactors();
        sf::Vector2f screenSize(
            normalizedSize.x * window.getSize().x,
            normalizedSize.y * window.getSize().y
        );
        
        // Update debug rectangle
        debugRect.setPosition(screenPos);
        debugRect.setSize(screenSize);
        debugRect.setFillColor(debugColor);
        
        window.draw(debugRect);
    }
} 