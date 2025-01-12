#pragma once

#include "GameState.hpp"
#include <SFML/Graphics.hpp>
#include <string>

struct OptionsUIPlacement {
    sf::Vector2f position;
    sf::Vector2f normalizedPosition;
};

class OptionsState : public GameState {
public:
    OptionsState();
    virtual ~OptionsState() = default;
    
    void init() override;
    void cleanup() override;
    void handleInput(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;

private:
    void loadUIPlacement();
    
    sf::Color backgroundColor;
    bool isTransitioningIn;
    bool isTransitioningOut;
    float transitionTime;
    float transitionDuration;
    
    // Animation related
    std::string currentAnimation;
    bool animationComplete;

    // UI Elements
    sf::Texture optionsButtonsTexture;
    sf::Sprite optionsButtonsSprite;
    sf::Texture resetGameButtonTexture;
    sf::Sprite resetGameButtonSprite;
    sf::Texture checkTexture;
    sf::Sprite checkSprite;
    
    // UI Placement
    OptionsUIPlacement optionsButtonsPlacement;
    OptionsUIPlacement resetGameButtonPlacement;
    OptionsUIPlacement checkPlacement;
}; 