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

private:
    MenuManager() : debugMode(false) {
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
    sf::Texture selector;
    sf::Sprite selectorSprite;
}; 