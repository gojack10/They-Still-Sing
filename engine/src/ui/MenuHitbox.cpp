#include "MenuHitbox.hpp"
#include "../systems/ui/ScalingManager.hpp"
#include <random>

MenuHitbox::MenuHitbox(const sf::Vector2f& absolutePosition, const sf::Vector2f& absoluteSize, 
                       const std::string& name, const sf::Vector2f& selectorPosition, bool hasSelector,
                       const std::string& state)
    : normalizedPosition(Engine::ScalingManager::absoluteToNormalized(absolutePosition))
    , normalizedSize(Engine::ScalingManager::absoluteToNormalized(absoluteSize))
    , selectorPosition(Engine::ScalingManager::absoluteToNormalized(selectorPosition))
    , name(name)
    , state(state)
    , hasSelector(hasSelector) {
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

void MenuHitbox::draw(sf::RenderWindow& window, bool debugMode, const std::string& currentState) const {
    // Only draw hitbox if we're in debug mode AND this hitbox belongs to the current state
    if (debugMode && state == currentState) {
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