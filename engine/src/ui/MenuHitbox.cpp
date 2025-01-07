#include "MenuHitbox.hpp"
#include <random>

MenuHitbox::MenuHitbox(const sf::FloatRect& bounds, const std::string& name)
    : bounds(bounds), name(name) {
    // Generate random color for debug visualization
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    debugColor = sf::Color(dis(gen), dis(gen), dis(gen), 204); // 80% opacity (204/255)
    
    // Setup debug rectangle
    debugRect.setSize(sf::Vector2f(bounds.width, bounds.height));
    debugRect.setPosition(bounds.left, bounds.top);
    debugRect.setFillColor(debugColor);
}

bool MenuHitbox::contains(const sf::Vector2f& point) const {
    return bounds.contains(point);
}

void MenuHitbox::draw(sf::RenderWindow& window, bool debugMode) const {
    if (debugMode) {
        window.draw(debugRect);
    }
} 