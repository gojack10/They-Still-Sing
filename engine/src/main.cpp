#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <numeric>
#include <deque>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float TARGET_FPS = 30.0f;
const float FRAME_TIME = 1.0f / TARGET_FPS;

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
    
    // For FPS calculation
    std::deque<float> frameTimes;
    const size_t FPS_SAMPLE_SIZE = 30;  // Calculate FPS over last 30 frames
    
    // Game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
        }

        // Update frame timing
        float elapsed = clock.restart().asSeconds();
        accumulator += elapsed;
        
        // Store frame time for FPS calculation
        frameTimes.push_back(elapsed);
        if (frameTimes.size() > FPS_SAMPLE_SIZE) {
            frameTimes.pop_front();
        }
        
        // Calculate and display FPS
        if (!frameTimes.empty()) {
            float avgFrameTime = std::accumulate(frameTimes.begin(), frameTimes.end(), 0.0f) / frameTimes.size();
            float currentFPS = 1.0f / avgFrameTime;
            
            // Check for dropped frames (if frame took significantly longer than target frame time)
            if (elapsed > FRAME_TIME * 1.2f) {  // 20% tolerance
                std::cout << "WARNING: Dropped frame! Frame time: " << std::fixed << std::setprecision(3) 
                          << elapsed * 1000.0f << "ms (target: " << FRAME_TIME * 1000.0f << "ms)" << std::endl;
            }
            
            // Print FPS every 30 frames
            if (currentFrame % 30 == 0) {
                std::cout << "FPS: " << std::fixed << std::setprecision(1) << currentFPS 
                          << " (Frame time: " << avgFrameTime * 1000.0f << "ms)" << std::endl;
            }
        }
        
        if (accumulator >= FRAME_TIME) {
            accumulator -= FRAME_TIME;
            currentFrame = (currentFrame + 1) % frames.size();
        }

        // Render
        window.clear(sf::Color::Black);
        window.draw(*sprites[currentFrame]);
        window.display();
    }

    return 0;
} 