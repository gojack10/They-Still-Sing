#include "MenuHitbox.hpp"
#include "../systems/ui/ScalingManager.hpp"
#include <random>

MenuHitbox::MenuHitbox(const sf::Vector2f& absolutePosition, const sf::Vector2f& absoluteSize, 
                       const std::string& name, const sf::Vector2f& selectorPosition, bool hasSelector,
                       const std::string& state, Engine::Anchor anchor)
    : normalizedPosition(Engine::ScalingManager::absoluteToNormalized(absolutePosition))
    , normalizedSize(Engine::ScalingManager::absoluteToNormalized(absoluteSize))
    , selectorPosition(Engine::ScalingManager::absoluteToNormalized(selectorPosition))
    , name(name)
    , state(state)
    , hasSelector(hasSelector)
    , anchor(anchor) {
    // Generate random color for debug visualization
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    debugColor = sf::Color(dis(gen), dis(gen), dis(gen), 204); // 80% opacity (204/255)
}

bool MenuHitbox::contains(const sf::Vector2f& normalizedPoint) const {
    auto& scalingManager = Engine::ScalingManager::getInstance();
    
    // Convert normalized position to screen coordinates using anchor
    sf::Vector2f screenPos = scalingManager.convertNormalizedToScreen(normalizedPosition.x, normalizedPosition.y, anchor);
    
    // Convert normalized size to screen size using scale factors
    sf::Vector2f scaleFactors = scalingManager.getScaleFactors();
    sf::Vector2f screenSize(
        normalizedSize.x * Engine::ScalingManager::BASE_WIDTH * scaleFactors.x,
        normalizedSize.y * Engine::ScalingManager::BASE_HEIGHT * scaleFactors.y
    );
    
    // Convert the test point to screen coordinates
    sf::Vector2f screenTestPoint = scalingManager.convertNormalizedToScreen(normalizedPoint.x, normalizedPoint.y);
    
    return screenTestPoint.x >= screenPos.x &&
           screenTestPoint.x <= screenPos.x + screenSize.x &&
           screenTestPoint.y >= screenPos.y &&
           screenTestPoint.y <= screenPos.y + screenSize.y;
}

void MenuHitbox::draw(sf::RenderWindow& window, bool debugMode, const std::string& currentState) const {
    // Only draw hitbox if we're in debug mode AND this hitbox belongs to the current state
    if (debugMode && state == currentState) {
        auto& scalingManager = Engine::ScalingManager::getInstance();
        
        // Convert normalized coordinates to screen coordinates with anchor
        sf::Vector2f screenPos = scalingManager.convertNormalizedToScreen(
            normalizedPosition.x, normalizedPosition.y, anchor);
        
        // Convert normalized size to screen size using scale factors
        sf::Vector2f scaleFactors = scalingManager.getScaleFactors();
        sf::Vector2f screenSize(
            normalizedSize.x * Engine::ScalingManager::BASE_WIDTH * scaleFactors.x,
            normalizedSize.y * Engine::ScalingManager::BASE_HEIGHT * scaleFactors.y
        );
        
        // Update debug rectangle
        sf::RectangleShape debugRect;
        debugRect.setPosition(screenPos);
        debugRect.setSize(screenSize);
        debugRect.setFillColor(debugColor);
        
        window.draw(debugRect);
    }
} 