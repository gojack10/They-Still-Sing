#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include "../systems/ui/ScalingManager.hpp"

class MenuHitbox {
public:
    MenuHitbox(const sf::Vector2f& absolutePosition, const sf::Vector2f& absoluteSize, 
               const std::string& name, const sf::Vector2f& selectorPosition, bool hasSelector,
               const std::string& state, Engine::Anchor anchor = Engine::Anchor::TopLeft);
    
    bool contains(const sf::Vector2f& normalizedPoint) const;
    void draw(sf::RenderWindow& window, bool debugMode, const std::string& currentState) const;
    
    const std::string& getName() const { return name; }
    const std::string& getState() const { return state; }
    const sf::Vector2f& getSelectorPosition() const { return selectorPosition; }
    bool getHasSelector() const { return hasSelector; }

private:
    sf::Vector2f normalizedPosition;
    sf::Vector2f normalizedSize;
    sf::Vector2f selectorPosition;
    std::string name;
    std::string state;
    bool hasSelector;
    mutable sf::RectangleShape debugRect;
    sf::Color debugColor;
    Engine::Anchor anchor;
}; 