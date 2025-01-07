#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class MenuHitbox {
public:
    MenuHitbox(const sf::Vector2f& absolutePosition, const sf::Vector2f& absoluteSize, const std::string& name);
    
    bool contains(const sf::Vector2f& normalizedPoint) const;
    void draw(sf::RenderWindow& window, bool debugMode) const;
    const std::string& getName() const { return name; }
    const sf::Vector2f& getNormalizedPosition() const { return normalizedPosition; }
    const sf::Vector2f& getNormalizedSize() const { return normalizedSize; }

private:
    sf::Vector2f normalizedPosition;  // Stored in normalized coordinates (0-1)
    sf::Vector2f normalizedSize;      // Stored in normalized coordinates (0-1)
    std::string name;
    sf::Color debugColor;
    mutable sf::RectangleShape debugRect;  // Mutable because we update it in draw()
}; 