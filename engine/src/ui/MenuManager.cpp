#include "MenuManager.hpp"
#include "../systems/ui/ScalingManager.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

void MenuManager::loadFromJson(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open menu configuration file: " + filepath);
    }

    nlohmann::json j;
    file >> j;

    hitboxes.clear();
    for (const auto& button : j["button_hitboxes"]) {
        // Convert JSON coordinates from normalized (0-1) to screen space
        sf::Vector2f normalizedPos(
            button["x"].get<float>(),
            button["y"].get<float>()
        );
        sf::Vector2f normalizedSize(
            button["w"].get<float>(),
            button["h"].get<float>()
        );

        // Handle optional selector field
        sf::Vector2f selectorPos(0.f, 0.f);
        bool hasSelector = false;
        if (button.contains("selector")) {
            const auto& selector = button["selector"];
            if (selector.contains("x") && selector.contains("y")) {
                selectorPos = sf::Vector2f(
                    selector["x"].get<float>(),
                    selector["y"].get<float>()
                );
                hasSelector = true;
            }
        }
        
        hitboxes.emplace_back(normalizedPos, normalizedSize, button["name"].get<std::string>(), selectorPos, hasSelector);
    }
}

void MenuManager::handleInput(const sf::RenderWindow& window) {
    auto& scalingManager = Engine::ScalingManager::getInstance();
    scalingManager.updateWindowSize(window.getSize().x, window.getSize().y);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
    sf::Vector2f normalizedPos = scalingManager.convertScreenToNormalized(worldPos.x, worldPos.y);

    hoveredButton.clear();
    for (const auto& hitbox : hitboxes) {
        if (hitbox.contains(normalizedPos)) {
            hoveredButton = hitbox.getName();
            // Only update selector position if the hitbox has a selector
            if (hitbox.getHasSelector()) {
                scalingManager.scaleSprite(selectorSprite, hitbox.getSelectorPosition());
            }
            break;
        }
    }
}

void MenuManager::draw(sf::RenderWindow& window) {
    auto& scalingManager = Engine::ScalingManager::getInstance();
    scalingManager.updateWindowSize(window.getSize().x, window.getSize().y);

    // Draw hitboxes in debug mode
    if (debugMode) {
        for (const auto& hitbox : hitboxes) {
            hitbox.draw(window, true);
        }
    }

    // Draw selector if a button is hovered and it has a selector
    if (!hoveredButton.empty()) {
        // Find the hovered hitbox
        auto it = std::find_if(hitboxes.begin(), hitboxes.end(),
            [this](const MenuHitbox& hitbox) { return hitbox.getName() == hoveredButton; });
        
        if (it != hitboxes.end() && it->getHasSelector()) {
            window.draw(selectorSprite);
        }
    }
} 