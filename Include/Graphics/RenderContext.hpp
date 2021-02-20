/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <stack>
#include <Core/ServiceStorage.hpp>
#include "Graphics/RenderState.hpp"

namespace System
{
    class Window;
}

/*
    Render Context

    Manages internal state of rendering system.
*/

namespace Graphics
{
    class RenderContext final : private Common::NonCopyable
    {
    public:
        struct CreateParams
        {
            const Core::ServiceStorage* services = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedStateCreation,
        };

        using CreateResult = Common::Result<std::unique_ptr<RenderContext>, CreateErrors>;
        static CreateResult Create(const CreateParams& params);

    public:
        ~RenderContext();

        void MakeCurrent();
        RenderState& PushState();
        RenderState& GetState();
        void PopState();

    private:
        RenderContext();

    private:
        System::Window* m_window = nullptr;

        RenderState m_currentState;
        std::stack<RenderState> m_pushedStates;
    };
}
