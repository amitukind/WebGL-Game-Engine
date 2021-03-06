/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Script/Precompiled.hpp"
#include "Script/ScriptState.hpp"
#include <Core/SystemStorage.hpp>
#include <System/FileSystem/FileHandle.hpp>
using namespace Script;

namespace
{
    extern "C"
    {
        // Prints string on top of stack to log stream.
        static int LuaLog(lua_State* L)
        {
            ASSERT(L != nullptr, "Lua state is null!");

            if(lua_isstring(L, -1))
            {
                LOG_INFO(lua_tostring(L, -1));
            }

            return 0;
        }
    }
}

ScriptState::ScriptState() = default;
ScriptState::~ScriptState()
{
    if(m_state)
    {
        lua_close(m_state);
    }
}

ScriptState::CreateResult ScriptState::Create()
{
    LOG_PROFILE_SCOPE("Create script state");

    // Create instance.
    auto instance = std::unique_ptr<ScriptState>(new ScriptState());

    // Create Lua state.
    instance->m_state = luaL_newstate();

    if(instance->m_state == nullptr)
    {
        LOG_ERROR("Could not create Lua state!");
        return Common::Failure(CreateErrors::FailedLuaStateCreation);
    }

    // Load base library.
    lua_pushcfunction(instance->m_state, luaopen_base);
    lua_pushstring(instance->m_state, "");

    if(lua_pcall(instance->m_state, 1, 0, 0) != 0)
    {
        LOG_ERROR("Could not load base Lua library!");
        instance->PrintError();
        return Common::Failure(CreateErrors::FailedLuaLibraryLoading);
    }

    // Register logging function.
    lua_pushcfunction(instance->m_state, LuaLog);
    lua_setglobal(instance->m_state, "Log");

    // Make sure that we did not leave anything on the stack.
    ASSERT(lua_gettop(instance->m_state) == 0, "Lua stack is not empty!");

    return Common::Success(std::move(instance));
}

ScriptState::CreateResult ScriptState::Create(System::FileHandle& file, const LoadFromFile& params)
{
    LOG_PROFILE_SCOPE("Load script state from \"{}\" file...",
        file.GetPath().generic_string());

    LOG("Loading script state from \"{}\" file...",
        file.GetPath().generic_string());

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(params.engineSystems,
        Common::Failure(CreateErrors::InvalidArgument));

    // Call base create method to retrieve new instance.
    auto createResult = Create();
    if(!createResult)
    {
        return createResult;
    }

    auto instance = createResult.Unwrap();

    // Execute script file.
    std::string scriptCode = file.ReadAsTextString();

    if(!instance->Execute(scriptCode))
    {
        LOG_ERROR("Could not execute script file!");
        instance->PrintError();
        return Common::Failure(CreateErrors::FailedLuaScriptExecution);
    }

    return Common::Success(std::move(instance));
}

bool ScriptState::Execute(std::string script)
{
    if(luaL_dostring(m_state, script.c_str()) != 0)
    {
        LOG_ERROR("Could not execute script!");
        PrintError();
        return false;
    }

    return true;
}

void ScriptState::PrintError()
{
    // Make sure that there is a string on top of the stack.
    ASSERT(lua_isstring(m_state, -1), "Expected a string!");

    // Print error string to log.
    LOG_DEBUG("Lua Error: {}", lua_tostring(m_state, -1));

    // Pop error string from stack.
    lua_pop(m_state, 1);
}

void ScriptState::CleanStack()
{
    // Discard remaining objects on the stack.
    int size = lua_gettop(m_state);

    if(size != 0)
    {
        LOG_DEBUG("Cleaning {} remaining objects on the stack...", size);
        lua_settop(m_state, 0);
    }
}

bool ScriptState::CollectGarbage(bool singleStep)
{
    if(singleStep)
    {
        // Perform only one step of the garbage collection process.
        // Return whether there is more garbage left to collect.
        return lua_gc(m_state, LUA_GCSTEP, 0) == 0;
    }
    else
    {
        // Immediately collect all garbage at once.
        lua_gc(m_state, LUA_GCCOLLECT, 0);
        return false;
    }
}

ScriptState::operator lua_State*()
{
    return m_state;
}
