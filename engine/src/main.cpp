#include <SFML/Graphics.hpp>

int main() {
    // Create a window with 1280x720 resolution
    sf::RenderWindow window(sf::VideoMode(1280, 720), "They Still Sing");
    
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

        // Clear the window with black color
        window.clear(sf::Color::Black);
        
        // Display the frame
        window.display();
    }

    return 0;
}
