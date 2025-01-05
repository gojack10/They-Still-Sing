#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>

int main() {
    // Create a window with 1280x720 resolution
    sf::RenderWindow window(sf::VideoMode(1280, 720), "They Still Sing");
    
    // Set frame limit to 30 FPS
    window.setFramerateLimit(30);

    // Load a font
    sf::Font font;
    if (!font.loadFromFile("engine/assets/fonts/DejaVuSans.ttf")) {
        // Fallback to default system font if loading fails
        return -1;
    }

    // Create FPS text
    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(24);
    fpsText.setFillColor(sf::Color::White);
    
    // Clock for FPS calculation
    sf::Clock clock;
    float lastTime = 0;
    
    // Main game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window if ESC key is pressed or window is closed
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }
        }

        // Calculate FPS
        float currentTime = clock.restart().asSeconds();
        float fps = 1.f / currentTime;
        // Round to nearest integer and clamp to 30 if we're very close
        int displayFps = static_cast<int>(std::round(fps));
        if (std::abs(fps - 30.0f) < 0.5f) {
            displayFps = 30;
        }

        // Update FPS text
        std::stringstream ss;
        ss << "FPS: " << displayFps;
        fpsText.setString(ss.str());
        
        // Position FPS counter in top right corner with some padding
        fpsText.setPosition(window.getSize().x - fpsText.getGlobalBounds().width - 10, 10);

        // Clear the window with black color
        window.clear(sf::Color::Black);
        
        // Draw FPS counter
        window.draw(fpsText);
        
        // Display the frame
        window.display();
    }

    return 0;
}
