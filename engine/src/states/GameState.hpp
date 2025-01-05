#pragma once

#include <SFML/Graphics.hpp>

class GameState {
public:
    virtual ~GameState() = default;
    virtual void init() = 0;
    virtual void handleInput(sf::RenderWindow& window) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
};
