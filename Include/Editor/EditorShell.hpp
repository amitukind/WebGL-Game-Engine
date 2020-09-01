/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Core/ServiceStorage.hpp>
#include "Editor/Modules/InputManagerEditor.hpp"
#include "Editor/Modules/GameInstanceEditor.hpp"

namespace Core
{
    class PerformanceMetrics;
};

namespace System
{
    class Window;
};

namespace Game
{
    class GameFramework;
}

/*
    Editor Shell

    Main front end class for editor interface.
*/

namespace Editor
{
    class EditorShell final : private Common::NonCopyable
    {
    public:
        struct CreateFromParams
        {
            const Core::ServiceStorage* services = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedModuleCreation,
        };

        using CreateResult = Common::Result<std::unique_ptr<EditorShell>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

        ~EditorShell();

        void Display(float timeDelta);

    private:
        EditorShell();

        bool CreateModules(const Core::ServiceStorage* services);
        void DisplayMenuBar();
        void DisplayFramerate();

        Core::PerformanceMetrics* m_performanceMetrics = nullptr;
        System::Window* m_window = nullptr;

        std::unique_ptr<InputManagerEditor> m_inputManagerEditor;
        std::unique_ptr<GameInstanceEditor> m_gameInstanceEditor;

        bool m_showDemoWindow = false;
    };
}
