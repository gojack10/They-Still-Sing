#include "MenuManager.hpp"
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
        sf::FloatRect bounds(
            button["x"].get<float>(),
            button["y"].get<float>(),
            button["w"].get<float>(),
            button["h"].get<float>()
        );
        hitboxes.emplace_back(bounds, button["name"].get<std::string>());
    }
}

void MenuManager::handleInput(const sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

    hoveredButton.clear();
    for (const auto& hitbox : hitboxes) {
        if (hitbox.contains(worldPos)) {
            hoveredButton = hitbox.getName();
            // Position selector sprite at hitbox position
            selectorSprite.setPosition(hitbox.getBounds().left, hitbox.getBounds().top);
            break;
        }
    }
}

void MenuManager::draw(sf::RenderWindow& window) {
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