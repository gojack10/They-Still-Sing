#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stdexcept>

// Include generated asset headers
#include "assets/fonts/DejaVuSans.hpp"
#include "assets/textures/ui/warning.hpp"

namespace AssetPaths {
    // Helper functions to load assets from memory
    inline sf::Font loadFont(const unsigned char* data, std::size_t size) {
        sf::Font font;
        if (!font.loadFromMemory(data, size)) {
            throw std::runtime_error("Failed to load font from memory");
        }
        return font;
    }

    inline sf::Texture loadTexture(const unsigned char* data, std::size_t size) {
        sf::Texture texture;
        if (!texture.loadFromMemory(data, size)) {
            throw std::runtime_error("Failed to load texture from memory");
        }
        return texture;
    }

    // Functions to get specific assets
    inline sf::Font getDejaVuSans() {
        return loadFont(EmbeddedAssets::fonts_dejavusans_ttf, 
                       EmbeddedAssets::fonts_dejavusans_ttf_size);
    }

    inline sf::Texture getWarningTexture() {
        return loadTexture(EmbeddedAssets::textures_ui_warning_jpg,
                          EmbeddedAssets::textures_ui_warning_jpg_size);
    }
} 