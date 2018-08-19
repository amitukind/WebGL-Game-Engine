/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <memory>

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Scene System
*/

namespace Game
{
    // Forward declarations.
    class Scene;

    // Scene system class.
    class SceneSystem
    {
    public:
        SceneSystem();
        ~SceneSystem();

        // Disallow copying.
        SceneSystem(const SceneSystem& other) = delete;
        SceneSystem& operator=(const SceneSystem& other) = delete;

        // Move constructor and assignment.
        SceneSystem(SceneSystem&& other);
        SceneSystem& operator=(SceneSystem&& other);

        // Initializes the scene system instance.
        bool Initialize(Engine::Root* engine);

        // Changes the current scene.
        void ChangeScene(std::shared_ptr<Scene> scene);

        // Updates the scene.
        void Update(float timeDelta);

        // Draws the scene.
        void Draw(float timeAlpha);

        // Checks if current scene has a custom editor implemented.
        bool HasCustomEditor() const;

    private:
        // Engine reference.
        Engine::Root* m_engine;

        // Current scene.
        std::shared_ptr<Scene> m_currentScene;

        // Initialization state.
        bool m_initialized;
    };
}
