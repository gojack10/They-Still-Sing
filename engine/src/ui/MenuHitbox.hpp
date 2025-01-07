#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class MenuHitbox {
public:
    MenuHitbox(const sf::FloatRect& bounds, const std::string& name);
    
    bool contains(const sf::Vector2f& point) const;
    void draw(sf::RenderWindow& window, bool debugMode) const;
    const std::string& getName() const { return name; }
    const sf::FloatRect& getBounds() const { return bounds; }

private:
    sf::FloatRect bounds;
    std::string name;
    sf::Color debugColor;
    sf::RectangleShape debugRect;
}; 