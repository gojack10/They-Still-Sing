#pragma once

#include "GameState.hpp"
#include <SFML/Graphics.hpp>
#include <string>

struct MenuTextPlacement {
    sf::Vector2f position;
    sf::Vector2f normalizedPosition;
};

class MainMenuState : public GameState {
public:
    MainMenuState();
    virtual ~MainMenuState() = default;
    
    void init() override;
    void handleInput(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;

private:
    void loadMenuTextPlacement();
    
    sf::Texture menuTextTexture;
    sf::Sprite menuTextSprite;
    MenuTextPlacement menuTextPlacement;
};
