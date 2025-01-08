#pragma once

#include "GameState.hpp"
#include <SFML/Graphics.hpp>

class WarningState : public GameState {
public:
    WarningState();
    virtual ~WarningState() = default;
    
    void init() override;
    void cleanup() override;
    void handleInput(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    
private:
    sf::Texture warningTexture;
    sf::Sprite warningSprite;
    sf::Clock timer;
    float opacity;
    float fadeTime = 5.0f;
    bool startFade = false;
    bool hasTransitioned = false;
}; 