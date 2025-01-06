#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <memory>
#include "states/WarningState.hpp"
#include "core/StateManager.hpp"
#include "config/AssetPaths.hpp"
#include "utils/Debug.hpp"

int main() {
    // Install signal handlers for debugging
    Debug::installSignalHandlers();
    
    try {
        // Create a window with 1280x720 resolution
        sf::RenderWindow window(sf::VideoMode(1280, 720), "They Still Sing");
        
        // Set frame limit to 30 FPS
        window.setFramerateLimit(30);

        // Load font from embedded assets
        sf::Font font;
        if (!font.loadFromFile(AssetPaths::DEJAVU_SANS)) {
            throw std::runtime_error("Failed to load DejaVu Sans font");
        }

        // Create FPS text
        sf::Text fpsText;
        fpsText.setFont(font);
        fpsText.setCharacterSize(24);
        fpsText.setFillColor(sf::Color::White);

        // Initialize state manager with warning state
        StateManager::getInstance().changeState(std::make_unique<WarningState>());
        
        // Clock for delta time calculation
        sf::Clock deltaClock;
        
        // Clock for FPS calculation
        sf::Clock fpsClock;
        
        // Main game loop
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }
            
            // Calculate delta time
            float deltaTime = deltaClock.restart().asSeconds();
            
            // Calculate FPS with smoothing
            float currentTime = fpsClock.getElapsedTime().asSeconds();
            static float lastTime = currentTime;
            static float fpsAccum = 0.0f;
            static int fpsFrames = 0;
            static int displayFps = 30;
            
            fpsFrames++;
            fpsAccum += 1.0f / (currentTime - lastTime);
            lastTime = currentTime;
            
            // Update displayed FPS every 0.5 seconds
            if (fpsFrames >= 15) {  // At 30 FPS, this is roughly 0.5 seconds
                displayFps = static_cast<int>(std::round(fpsAccum / fpsFrames));
                // Clamp to 30 if we're very close
                if (std::abs(displayFps - 30.0f) < 2.0f) {
                    displayFps = 30;
                }
                fpsFrames = 0;
                fpsAccum = 0.0f;
            }

            // Clear the window
            window.clear();

            // Update FPS text
            std::stringstream ss;
            ss << "FPS: " << displayFps;
            fpsText.setString(ss.str());
            
            // Position FPS counter in top right corner with some padding
            fpsText.setPosition(window.getSize().x - fpsText.getGlobalBounds().width - 10, 10);
            
            // Update and draw current state
            auto& stateManager = StateManager::getInstance();
            if (auto* state = stateManager.getCurrentState()) {
                state->handleInput(window);
            }
            if (auto* state = stateManager.getCurrentState()) {
                state->update(deltaTime);
            }
            if (auto* state = stateManager.getCurrentState()) {
                state->draw(window);
            }
            
            // Draw FPS counter on top
            window.draw(fpsText);
            window.display();
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        Debug::printBacktrace();
        return 1;
    } catch (...) {
        std::cerr << "Fatal error: Unknown exception" << std::endl;
        Debug::printBacktrace();
        return 1;
    }
    
    return 0;
}
