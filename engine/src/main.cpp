#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <memory>
#include "states/WarningState.hpp"
#include "core/StateManager.hpp"
#include "config/AssetPaths.hpp"
#include "ui/MenuManager.hpp"
#include "systems/ui/ScalingManager.hpp"
#include "systems/audio_systems/AudioSystem.hpp"

const unsigned int BASE_WIDTH = 1280;
const unsigned int BASE_HEIGHT = 720;

void updateView(sf::RenderWindow& window) {
    // Update ScalingManager with new window size
    Engine::ScalingManager::getInstance().updateWindowSize(window.getSize().x, window.getSize().y);
    
    // Create a view that covers the entire window
    sf::View view;
    view.setSize(window.getSize().x, window.getSize().y);
    view.setCenter(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
    window.setView(view);
}

int main() {
    try {
        // Create a window with 1280x720 resolution
        sf::RenderWindow window(sf::VideoMode(BASE_WIDTH, BASE_HEIGHT), "They Still Sing", sf::Style::Default | sf::Style::Resize);
        bool isFullscreen = false;
        bool showHitboxes = false;
        
        // Initialize ScalingManager with base window size
        Engine::ScalingManager::getInstance().updateWindowSize(BASE_WIDTH, BASE_HEIGHT);
        
        // Initialize AudioSystem
        auto& audio = Engine::AudioSystem::getInstance();
        audio.initialize(AssetPaths::AUDIO_CONFIG);
        audio.setDebugEnabled(false);  // Disable debug output
        
        // Set up menu music transition callback
        bool menuLoopStarted = false;
        audio.setMusicStopCallback([&audio, &menuLoopStarted](const std::string& musicName) {
            if (musicName == "menu-start" && !menuLoopStarted) {
                audio.playMusic("menu-loop");
                menuLoopStarted = true;
            }
        });
        
        // Start menu intro music
        audio.playMusic("menu-start");
        
        // Set frame limit to 30 FPS
        window.setFramerateLimit(30);

        // Load font from embedded assets
        sf::Font font;
        if (!font.loadFromFile(AssetPaths::OCRAEXT)) {
            throw std::runtime_error("Failed to load OCRAEXT font");
        }

        // Create FPS text
        sf::Text fpsText;
        fpsText.setFont(font);
        fpsText.setCharacterSize(30);
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
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                else if (event.type == sf::Event::KeyPressed) {
                    switch (event.key.code) {
                        case sf::Keyboard::Escape:
                            window.close();
                            break;
                        case sf::Keyboard::F:
                            isFullscreen = !isFullscreen;
                            if (isFullscreen) {
                                window.create(sf::VideoMode::getDesktopMode(), "They Still Sing", sf::Style::Fullscreen);
                            } else {
                                window.create(sf::VideoMode(BASE_WIDTH, BASE_HEIGHT), "They Still Sing", sf::Style::Default | sf::Style::Resize);
                            }
                            window.setFramerateLimit(30);
                            updateView(window);
                            break;
                        case sf::Keyboard::D:
                            showHitboxes = !showHitboxes;
                            MenuManager::getInstance().toggleDebugMode();
                            break;
                        default:
                            break;
                    }
                }
                else if (event.type == sf::Event::Resized) {
                    updateView(window);
                    // Scale FPS text size using ScalingManager
                    fpsText.setCharacterSize(static_cast<unsigned int>(
                        Engine::ScalingManager::getInstance().getScaledFontSize(20.0f)
                    ));
                }
            }
            
            // Calculate delta time
            float deltaTime = deltaClock.restart().asSeconds();
            
            // Update audio system
            audio.update(deltaTime);

            // Calculate FPS with smoothing
            float currentTime = fpsClock.getElapsedTime().asSeconds();
            static float lastTime = currentTime;
            static float fpsAccum = 0.0f;
            static int fpsFrames = 0;
            static int displayFps = 30;
            
            fpsFrames++;
            fpsAccum += 1.0f / (currentTime - lastTime);
            lastTime = currentTime;
            
            if (fpsFrames >= 15) {
                displayFps = static_cast<int>(std::round(fpsAccum / fpsFrames));
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
            
            // Convert absolute coordinates from menu_config.json to normalized coordinates
            sf::Vector2f normalizedPos = Engine::ScalingManager::absoluteToNormalized(1137.0f, 20.0f);
            sf::Vector2f fpsPos = Engine::ScalingManager::getInstance().convertNormalizedToScreen(
                normalizedPos.x, normalizedPos.y, Engine::Anchor::TopLeft
            );
            fpsText.setPosition(fpsPos);
            
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

        // Stop all music before closing
        audio.stopMusic("menu-start");
        audio.stopMusic("menu-loop");
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Fatal error: Unknown exception" << std::endl;
        return 1;
    }
    
    return 0;
}
