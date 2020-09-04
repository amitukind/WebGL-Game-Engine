/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Game/Precompiled.hpp"
#include "Game/GameFramework.hpp"
#include "Game/GameInstance.hpp"
#include <System/Window.hpp>
#include <Renderer/GameRenderer.hpp>
using namespace Game;

GameFramework::GameFramework() = default;
GameFramework::~GameFramework() = default;

GameFramework::CreateResult GameFramework::Create(const CreateFromParams& params)
{
    LOG("Creating game framework...");
    LOG_SCOPED_INDENT();

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(params.services != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<GameFramework>(new GameFramework());

    // Save system references.
    instance->m_timer = params.services->GetTimer();
    instance->m_window = params.services->GetWindow();
    instance->m_gameRenderer = params.services->GetGameRenderer();

    // Success!
    return Common::Success(std::move(instance));
}

GameFramework::ProcessGameStateResults GameFramework::ProcessGameState(float timeDelta)
{
    // Acquire current state and its parts.
    std::shared_ptr<GameState> currentState = m_stateMachine.GetState();
    TickTimer* tickTimer = currentState ? currentState->GetTickTimer() : nullptr;
    GameInstance* gameInstance = currentState ? currentState->GetGameInstance() : nullptr;

    // Track whether tick was processed.
    bool tickProcessed = false;

    // Process current game state.
    if(currentState)
    {
        // Process tick timer.
        if(tickTimer)
        {
            tickTimer->Advance(*m_timer);
        }

        // Inform about tick being requested.
        events.tickRequested.Dispatch();

        // Process game tick.
        // Tick may be processed multiple times if behind the schedule.
        while(!tickTimer || tickTimer->Tick())
        {
            // Determine tick time.
            float tickTime = tickTimer ? tickTimer->GetLastTickSeconds() : timeDelta;

            // Pre tick game instance.
            if(gameInstance)
            {
                gameInstance->PreTick(tickTime);
            }

            // Call game state tick method.
            currentState->Tick(tickTime);

            // Post tick game instance.
            if(gameInstance)
            {
                gameInstance->PostTick(tickTime);
            }

            // Inform that tick has been processed.
            events.tickProcessed.Dispatch(tickTime);

            // Mark tick as processed.
            tickProcessed = true;

            // Tick only once if there is no tick timer.
            if(!tickTimer)
                break;
        }

        // Call game state update method.
        currentState->Update(timeDelta);

        // Determine time alpha.
        float timeAlpha = tickTimer ? tickTimer->GetAlphaSeconds() : 1.0f;

        // Draw game instance.
        if(gameInstance)
        {
            glm::ivec4 viewportRect = { 0, 0, m_window->GetWidth(), m_window->GetHeight() };

            Renderer::GameRenderer::DrawParams drawParams;
            drawParams.viewportRect = viewportRect;
            drawParams.gameInstance = gameInstance;
            drawParams.cameraName = "Camera";
            drawParams.timeAlpha = timeAlpha;
            m_gameRenderer->Draw(drawParams);
        }

        // Call game state draw method.
        currentState->Draw(timeAlpha);
    }

    // Return whether tick was processed.
    return tickProcessed ? ProcessGameStateResults::TickedAndUpdated : ProcessGameStateResults::UpdatedOnly;
}

GameFramework::ChangeGameStateResult GameFramework::ChangeGameState(std::shared_ptr<GameState> gameState)
{
    // Make sure we are not changing into current game state.
    if(gameState == m_stateMachine.GetState())
    {
        LOG_WARNING("Attempted to change into current game state!");
        return Common::Failure(ChangeGameStateErrors::AlreadyCurrent);
    }

    // Change into new game state.
    if(!m_stateMachine.ChangeState(gameState))
        return Common::Failure(ChangeGameStateErrors::FailedTransition);

    // Notify listeners about game state transition.
    events.gameStateChanged.Dispatch(gameState);

    // State transition succeeded.
    return Common::Success();
}

bool GameFramework::HasGameState() const
{
    return m_stateMachine.HasState();
}
