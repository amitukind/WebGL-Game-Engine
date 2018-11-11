/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Game/EntitySystem.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/IdentitySystem.hpp"
#include "Game/Systems/SpriteAnimationSystem.hpp"

// Forward declaration.
namespace Engine
{
    class Root;
}

/*
    Game State
*/

namespace Game
{
    // Game state class.
    class GameState
    {
    public:
        GameState();
        ~GameState();

        GameState(const GameState& other) = delete;
        GameState& operator=(const GameState& other) = delete;

        GameState(GameState&& other);
        GameState& operator=(GameState&& other);

        // Initializes the game state.
        bool Initialize(Engine::Root* engine);

        // Updates the game state.
        void Update(float timeDelta);

        // Gets the engine reference.
        Engine::Root* GetEngine() const;

    public:
        // Core game systems.
        Game::EntitySystem entitySystem;
        Game::ComponentSystem componentSystem;
        Game::IdentitySystem identitySystem;

        // Base game systems.
        Game::SpriteAnimationSystem spriteAnimationSystem;

    private:
        // Engine reference.
        Engine::Root* m_engine;

        // Initialization state.
        bool m_initialized;
    };
}
