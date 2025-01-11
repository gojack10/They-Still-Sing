#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include <unistd.h>
#include <linux/limits.h>
#include <libgen.h>

namespace AssetPaths {
    namespace {
        // Get the executable's directory
        inline std::string getExecutableDir() {
            char result[PATH_MAX];
            ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
            if (count != -1) {
                result[count] = '\0';
                char* path = dirname(result);
                return std::string(path);
            }
            return "";
        }

        // Internal helper to resolve paths at runtime
        inline std::string resolvePath(const std::string& path) {
            std::string exeDir = getExecutableDir();
            
            // First try relative to executable
            std::string exePath = exeDir + "/" + path;
            if (std::filesystem::exists(exePath)) {
                return exePath;
            }
            
            // Then try relative to current directory
            if (std::filesystem::exists(path)) {
                return path;
            }
            
            // If not found, try the development path
            std::string devPath = "engine/" + path;
            if (std::filesystem::exists(devPath)) {
                return devPath;
            }
            
            // Return the exe path as default (error will be handled by the loading function)
            return exePath;
        }
    }

    // Base paths
    const std::string ASSETS_DIR = "assets";
    const std::string TEXTURES_DIR = ASSETS_DIR + "/textures";
    const std::string FONTS_DIR = ASSETS_DIR + "/fonts";
    
    // Audio paths
    const std::string AUDIO_DIR = ASSETS_DIR + "/sound";
    const std::string MUSIC_DIR = AUDIO_DIR + "/music";
    const std::string SFX_DIR = AUDIO_DIR + "/sfx";
    const std::string UI_SFX_DIR = SFX_DIR + "/ui";
    
    // Music files
    const std::string MENU_MUSIC_START = resolvePath(MUSIC_DIR + "/menu-music-start.ogg");
    const std::string MENU_MUSIC_LOOP = resolvePath(MUSIC_DIR + "/menu-music-loop.ogg");
    
    // Sound files
    const std::string MENU_HOVER_SOUND = resolvePath(UI_SFX_DIR + "/menu-hover.ogg");
    
    // Animation paths
    const std::string ANIMATIONS_DIR = TEXTURES_DIR + "/animations";
    const std::string MAIN_MENU_ANIM = resolvePath(ANIMATIONS_DIR + "/main-menu-anim");
    const std::string OPTIONS_ENTER_ANIM = resolvePath(ANIMATIONS_DIR + "/options-enter");
    const std::string OPTIONS_EXIT_ANIM = resolvePath(ANIMATIONS_DIR + "/options-exit");
    
    // UI paths
    const std::string UI_DIR = TEXTURES_DIR + "/ui";
    const std::string WARNING_TEXTURE = resolvePath(UI_DIR + "/warning.jpg");
    const std::string MENU_TEXT_TEXTURE = resolvePath(UI_DIR + "/menu-text.png");
    
    // Font paths
    const std::string DEJAVU_SANS = resolvePath(FONTS_DIR + "/DejaVuSans.ttf");
    const std::string OCRAEXT = resolvePath(FONTS_DIR + "/OCRAEXT.ttf");
    
    // UI Elements
    inline const std::string UI_SELECTOR = resolvePath(TEXTURES_DIR + "/ui/selector.png");
    inline const std::string MENU_CONFIG = resolvePath(ASSETS_DIR + "/config/menu_config.json");
    
    // Audio config
    inline const std::string AUDIO_CONFIG = resolvePath(ASSETS_DIR + "/config/audio_config.json");
} 