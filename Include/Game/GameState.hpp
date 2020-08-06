/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <any>
#include <Event/Queue.hpp>
#include <Event/Broker.hpp>
#include "Game/UpdateTimer.hpp"
#include "Game/EntitySystem.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/Systems/IdentitySystem.hpp"
#include "Game/Systems/InterpolationSystem.hpp"
#include "Game/Systems/SpriteSystem.hpp"

/*
    Game State
*/

namespace Game
{
    class GameState final : private Common::NonCopyable
    {
    public:
        enum class CreateErrors
        {
            FailedSubsystemCreation,
        };

        using CreateResult = Common::Result<std::unique_ptr<GameState>, CreateErrors>;
        static CreateResult Create();

    public:
        ~GameState();

        bool Update(const System::Timer& timer);
        void ChangeUpdateTime(float updateTime);
        float GetUpdateTime() const;

    public:
        std::unique_ptr<UpdateTimer> updateTimer;
        std::unique_ptr<EntitySystem> entitySystem;
        std::unique_ptr<ComponentSystem> componentSystem;

        std::unique_ptr<IdentitySystem> identitySystem;
        std::unique_ptr<InterpolationSystem> interpolationSystem;
        std::unique_ptr<SpriteSystem> spriteSystem;

        struct Events
        {
            // Called when the class instance is destructed.
            Event::Dispatcher<void()> instanceDestructed;

            // Called when update method is called.
            // This does not mean that the state will be actually updated.
            Event::Dispatcher<void()> updateCalled;

            // Called when state had its update processed.
            // Event can be dispatched multiple times during the same update call.
            // This is also good time to run custom update logic.
            Event::Dispatcher<void(float)> updateProcessed;
        } events;

    private:
        GameState();

        float m_updateTime = 1.0f / 10.0f;
    };
}
