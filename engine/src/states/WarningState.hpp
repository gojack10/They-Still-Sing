#pragma once

#include <SFML/Graphics.hpp>
#include "GameState.hpp"

class WarningState : public GameState {
private:
    sf::Texture warningTexture;
    sf::Sprite warningSprite;
    sf::Clock timer;
    float fadeTime = 5.0f; // 5 seconds before fade starts
    bool startFade = false;
    float opacity = 255.0f;

public:
    WarningState();
    virtual void init() override;
    virtual void handleInput(sf::RenderWindow& window) override;
    virtual void update(float deltaTime) override;
    virtual void draw(sf::RenderWindow& window) override;
}; 