#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include "MenuHitbox.hpp"
#include "../config/AssetPaths.hpp"

class MenuManager {
public:
    static MenuManager& getInstance() {
        static MenuManager instance;
        return instance;
    }

    void loadFromJson(const std::string& filepath);
    void handleInput(const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    void toggleDebugMode() { debugMode = !debugMode; }
    bool isDebugMode() const { return debugMode; }
    const std::string& getHoveredButton() const { return hoveredButton; }
    void setCurrentState(const std::string& state) { currentState = state; }
    const std::string& getCurrentState() const { return currentState; }
    void clearHitboxes() { hitboxes.clear(); }
    bool isHitboxClicked(const std::string& name, const sf::RenderWindow& window) const;

private:
    MenuManager() : debugMode(false), currentState("MainMenu") {
        if (!selector.loadFromFile(AssetPaths::UI_SELECTOR)) {
            throw std::runtime_error("Failed to load selector texture");
        }
        selectorSprite.setTexture(selector);
    }
    MenuManager(const MenuManager&) = delete;
    MenuManager& operator=(const MenuManager&) = delete;

    std::vector<MenuHitbox> hitboxes;
    bool debugMode;
    std::string hoveredButton;
    std::string currentState;
    sf::Texture selector;
    sf::Sprite selectorSprite;
}; 