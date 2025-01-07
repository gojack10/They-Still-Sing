#pragma once

#include <string>

namespace AssetPaths {
    // Base paths
    const std::string ASSETS_DIR = "engine/assets";
    const std::string TEXTURES_DIR = ASSETS_DIR + "/textures";
    const std::string FONTS_DIR = ASSETS_DIR + "/fonts";
    
    // Animation paths
    const std::string ANIMATIONS_DIR = TEXTURES_DIR + "/animations";
    const std::string MAIN_MENU_ANIM = ANIMATIONS_DIR + "/main-menu-anim";
    
    // UI paths
    const std::string UI_DIR = TEXTURES_DIR + "/ui";
    const std::string WARNING_TEXTURE = UI_DIR + "/warning.jpg";
    const std::string MENU_TEXT_TEXTURE = UI_DIR + "/menu-text.png";
    
    // Font paths
    const std::string DEJAVU_SANS = FONTS_DIR + "/DejaVuSans.ttf";
    const std::string OCRAEXT = FONTS_DIR + "/OCRAEXT.ttf";
    
    // UI Elements
    constexpr const char* UI_SELECTOR = "engine/assets/textures/ui/selector.png";
    constexpr const char* MENU_CONFIG = "engine/assets/config/menu_config.json";
} 