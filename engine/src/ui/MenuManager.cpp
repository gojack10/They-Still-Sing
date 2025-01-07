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
    for (const auto& button : j["buttons"]) {
        // Convert JSON coordinates from normalized (0-1) to screen space
        sf::Vector2f normalizedPos(
            button["x"].get<float>(),
            button["y"].get<float>()
        );
        sf::Vector2f normalizedSize(
            button["w"].get<float>(),
            button["h"].get<float>()
        );
        
        hitboxes.emplace_back(normalizedPos, normalizedSize, button["name"].get<std::string>());
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
            // Position selector sprite at hitbox position using normalized coordinates
            sf::Vector2f hitboxPos = hitbox.getNormalizedPosition();
            scalingManager.scaleSprite(selectorSprite, hitboxPos);
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

    // Draw selector if a button is hovered
    if (!hoveredButton.empty()) {
        window.draw(selectorSprite);
    }
} 