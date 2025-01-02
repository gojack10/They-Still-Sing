#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float FPS = 30.0f;
const float FRAME_TIME = 1.0f / FPS;

int main() {
    // Create window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "They Still Sing");
    window.setFramerateLimit(30);

    // Load frames
    std::vector<sf::Texture> frames;
    std::vector<sf::Sprite> sprites;
    
    // Load 6 frames
    for (int i = 1; i <= 6; i++) {
        sf::Texture texture;
        std::stringstream ss;
        ss << "../frames/desk-idle/" << std::setfill('0') << std::setw(4) << i << ".jpg";
        std::string path = ss.str();
        
        if (!texture.loadFromFile(path)) {
            std::cerr << "Failed to load frame: " << path << std::endl;
            return -1;
        }
        
        frames.push_back(texture);
        sprites.emplace_back(frames.back());
        
        // Center the sprite
        sprites.back().setPosition(
            (WINDOW_WIDTH - sprites.back().getGlobalBounds().width) / 2,
            (WINDOW_HEIGHT - sprites.back().getGlobalBounds().height) / 2
        );
    }

    int currentFrame = 0;
    sf::Clock clock;

    // Game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Update frame
        if (clock.getElapsedTime().asSeconds() >= FRAME_TIME) {
            currentFrame = (currentFrame + 1) % frames.size();
            clock.restart();
        }

        // Render
        window.clear(sf::Color::Black);
        window.draw(sprites[currentFrame]);
        window.display();
    }

    return 0;
} 