/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Interpolation System
*/

namespace Game
{
    // Forward declarations.
    class ComponentSystem;

    // Interpolation system class.
    class InterpolationSystem : private NonCopyable
    {
    public:
        InterpolationSystem();
        ~InterpolationSystem();

        InterpolationSystem(InterpolationSystem&& other);
        InterpolationSystem& operator=(InterpolationSystem&& other);

        // Initializes the interpolation system.
        bool Initialize(ComponentSystem* componentSystem);

        // Updates the interpolation system.
        void Update(float timeDelta);

    private:
        // Component system reference.
        ComponentSystem* m_componentSystem;

        // Initialization state.
        bool m_initialized;
    };
}
