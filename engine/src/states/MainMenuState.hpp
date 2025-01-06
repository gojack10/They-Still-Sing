#pragma once

#include "GameState.hpp"
#include <SFML/Graphics.hpp>

class MainMenuState : public GameState {
public:
    MainMenuState();
    virtual ~MainMenuState() = default;
    
    void init() override;
    void handleInput(sf::RenderWindow& window) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
};
