#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include "states/WarningState.hpp"
#include "config/AssetPaths.hpp"

int main() {
    // Create a window with 1280x720 resolution
    sf::RenderWindow window(sf::VideoMode(1280, 720), "They Still Sing");
    
    // Set frame limit to 30 FPS
    window.setFramerateLimit(30);

    // Load a font
    sf::Font font;
    if (!font.loadFromFile(AssetPaths::FONTS + "/DejaVuSans.ttf")) {
        // Fallback to default system font if loading fails
        return -1;
    }

    // Create FPS text
    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(24);
    fpsText.setFillColor(sf::Color::White);

    // Create warning state
    WarningState warningState;
    
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
        
        warningState.handleInput(window);
        warningState.update(deltaTime);
        warningState.draw(window);
        
        // Draw FPS counter on top
        window.draw(fpsText);
        window.display();
    }

    return 0;
}
