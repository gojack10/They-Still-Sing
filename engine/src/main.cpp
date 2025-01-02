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
    std::vector<std::unique_ptr<sf::Texture>> frames;
    std::vector<std::unique_ptr<sf::Sprite>> sprites;
    
    // Load 6 frames
    for (int i = 1; i <= 6; i++) {
        auto texture = std::make_unique<sf::Texture>();
        std::stringstream ss;
        ss << "../../engine/frames/desk-idle/" << std::setfill('0') << std::setw(4) << i << ".jpg";
        std::string path = ss.str();
        
        if (!texture->loadFromFile(path)) {
            std::cerr << "Failed to load frame: " << path << std::endl;
            return -1;
        }
        
        texture->setSmooth(true);  // Enable smooth texture filtering
        frames.push_back(std::move(texture));
        
        auto sprite = std::make_unique<sf::Sprite>();
        sprite->setTexture(*frames.back(), true);  // True ensures the texture is immediately bound
        
        // Center the sprite
        sprite->setPosition(
            (WINDOW_WIDTH - sprite->getGlobalBounds().width) / 2,
            (WINDOW_HEIGHT - sprite->getGlobalBounds().height) / 2
        );
        
        sprites.push_back(std::move(sprite));
    }

    int currentFrame = 0;
    sf::Clock clock;
    float accumulator = 0.0f;

    // Game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Update frame with smoother timing
        float elapsed = clock.restart().asSeconds();
        accumulator += elapsed;
        
        if (accumulator >= FRAME_TIME) {
            accumulator -= FRAME_TIME;  // Only subtract the frame time we've used
            int previousFrame = currentFrame;
            currentFrame = (currentFrame + 1) % frames.size();
            std::cout << "Frame transition: " << previousFrame << " -> " << currentFrame 
                      << " (dt: " << elapsed << "s)" << std::endl;
        }

        // Render
        window.clear(sf::Color::Black);
        window.draw(*sprites[currentFrame]);
        window.display();
    }

    return 0;
} 