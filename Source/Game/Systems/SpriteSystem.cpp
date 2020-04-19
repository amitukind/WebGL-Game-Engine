/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Game/Systems/SpriteSystem.hpp"
#include "Game/Components/SpriteAnimationComponent.hpp"
#include "Game/Components/SpriteComponent.hpp"
#include "Game/ComponentSystem.hpp"
#include <Graphics/Sprite/SpriteAnimationList.hpp>
using namespace Game;

SpriteSystem::SpriteSystem() = default;
SpriteSystem::~SpriteSystem() = default;

SpriteSystem::CreateResult SpriteSystem::Create(ComponentSystem* componentSystem)
{
    LOG("Create sprite system...");
    LOG_SCOPED_INDENT();

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(componentSystem != nullptr, Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<SpriteSystem>(new SpriteSystem());

    // Save component system reference.
    instance->m_componentSystem = componentSystem;

    // Success!
    return Success(std::move(instance));
}

void SpriteSystem::Update(float timeDelta)
{
    // Get all sprite animation components.
    for(auto& spriteAnimationComponent : m_componentSystem->GetPool<SpriteAnimationComponent>())
    {
        // Update sprite animation component.
        spriteAnimationComponent.Update(timeDelta);
    }
}
