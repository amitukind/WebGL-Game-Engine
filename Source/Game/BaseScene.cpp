/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/BaseScene.hpp"
#include "Engine.hpp"
using namespace Game;

BaseScene::BaseScene() :
    m_engine(nullptr),
    m_initialized(false)
{
}

BaseScene::~BaseScene()
{
}

BaseScene::BaseScene(BaseScene&& other)
{
    // Call the move assignment.
    *this = std::move(other);
}

BaseScene& BaseScene::operator=(BaseScene&& other)
{
    // Swap class members.
    std::swap(entitySystem, other.entitySystem);
    std::swap(componentSystem, other.componentSystem);
    std::swap(identitySystem, other.identitySystem);

    std::swap(spriteAnimationSystem, other.spriteAnimationSystem);

    std::swap(m_engine, other.m_engine);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool BaseScene::Initialize(Engine::Root* engine)
{
    LOG() << "Initializing base scene..." << LOG_INDENT();

    // Check if base game scene class has been already initialized.
    VERIFY(!m_initialized, "Base scene has already been initialized!");

    // Reset class instance on initialization failure.
    SCOPE_GUARD_IF(!m_initialized, *this = BaseScene());

    // Validate engine reference.
    if(engine == nullptr || !engine->IsInitialized())
    {
        LOG_ERROR() << "Invalid argument - \"engine\" is invalid!";
        return false;
    }

    m_engine = engine;

    // Initialize the entity system.
    // Assigns unique identifiers that all other systems use to identify objects in a game.
    if(!entitySystem.Initialize())
    {
        LOG_ERROR() << "Could not initialize entity system!";
        return false;
    }

    // Initialize the component system.
    // Stores and manages components that entities have.
    if(!componentSystem.Initialize(&entitySystem))
    {
        LOG_ERROR() << "Could not initialize component system!";
        return false;
    }

    // Initialize the identity system.
    // Allows readable names to be assigned to entities.
    if(!identitySystem.Initialize(&entitySystem))
    {
        LOG_ERROR() << "Could not initialize identity system!";
        return false;
    }

    // Initialize the sprite animation system.
    // Updates sprites according to their sprite animations.
    if(!spriteAnimationSystem.Initialize(&componentSystem))
    {
        LOG_ERROR() << "Could not initialize sprite animation system!";
        return false;
    }

    // Success!
    return m_initialized = true;
}

void BaseScene::OnUpdate(float timeDelta)
{
    ASSERT(m_initialized, "Base scene class has not been initialized!");

    // Process entity commands.
    entitySystem.ProcessCommands();

    // Update the sprite animation system.
    spriteAnimationSystem.Update(timeDelta);
}

void BaseScene::OnDraw(const SceneDrawParams& drawParams)
{
    ASSERT(m_initialized, "Base scene class has not been initialized!");
}

Engine::Root* BaseScene::GetEngine() const
{
    ASSERT(m_initialized, "Base scene class has not been initialized!");

    return m_engine;
}
