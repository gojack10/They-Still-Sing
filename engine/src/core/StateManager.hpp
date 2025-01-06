#pragma once

#include "../states/GameState.hpp"
#include <memory>

class StateManager {
public:
    static StateManager& getInstance() {
        static StateManager instance;
        return instance;
    }
    
    void changeState(std::unique_ptr<GameState> newState) {
        currentState = std::move(newState);
    }
    
    GameState* getCurrentState() { return currentState.get(); }
    
private:
    StateManager() = default;
    ~StateManager() = default;
    
    StateManager(const StateManager&) = delete;
    StateManager& operator=(const StateManager&) = delete;
    
    std::unique_ptr<GameState> currentState;
};
