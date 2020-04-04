/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <queue>
#include <System/Window.hpp>

/*
    Input Manager Editor
*/

namespace Editor
{
    // Input manager editor class.
    class InputManagerEditor
    {
    public:
        struct InitializeFromParams
        {
            System::Window* window = nullptr;
        };

    public:
        InputManagerEditor();
        ~InputManagerEditor();

        // Disallow copying.
        InputManagerEditor(const InputManagerEditor& other) = delete;
        InputManagerEditor& operator=(const InputManagerEditor& other) = delete;

        // Move constructor and assignment.
        InputManagerEditor(InputManagerEditor&& other);
        InputManagerEditor& operator=(InputManagerEditor&& other);

        // Initializes input manager editor.
        bool Initialize(const InitializeFromParams& params);

        // Updates input manager editor.
        void Update(float timeDelta);

    public:
        // Window state.
        bool mainWindowOpen = false;

    private:
        // Adds an incoming event log text.
        void AddIncomingEventLog(std::string text);

        // Input event handlers.
        void OnWindowFocus(const System::Window::Events::Focus& event);
        bool OnKeyboardKey(const System::Window::Events::KeyboardKey& event);
        bool OnTextInput(const System::Window::Events::TextInput& event);
        bool OnMouseButton(const System::Window::Events::MouseButton& event);
        bool OnMouseScroll(const System::Window::Events::MouseScroll& event);
        void OnCursorPosition(const System::Window::Events::CursorPosition& event);
        void OnCursorEnter(const System::Window::Events::CursorEnter& event);

    private:
        // System references.
        System::Window* m_window = nullptr;

        // Incoming event log.
        Event::Receiver<void(const System::Window::Events::Focus&)> m_windowFocusReceiver;
        Event::Receiver<bool(const System::Window::Events::KeyboardKey&)> m_keyboardKeyReceiver;
        Event::Receiver<bool(const System::Window::Events::TextInput&)> m_textInputReceiver;
        Event::Receiver<bool(const System::Window::Events::MouseButton&)> m_mouseButtonReceiver;
        Event::Receiver<bool(const System::Window::Events::MouseScroll&)> m_mouseScrollReceiver;
        Event::Receiver<void(const System::Window::Events::CursorPosition&)> m_cursorPositionReceiver;
        Event::Receiver<void(const System::Window::Events::CursorEnter&)> m_cursorEnterReceiver;

        bool m_incomingEventFreeze = false;
        bool m_incomingWindowFocus = false;
        bool m_incomingKeyboardKey = false;
        bool m_incomingKeyboardKeyPress = false;
        bool m_incomingKeyboardKeyRelease = false;
        bool m_incomingKeyboardKeyRepeat = false;
        bool m_incomingTextInput = false;
        bool m_incomingMouseButton = false;
        bool m_incomingMouseButtonPress = false;
        bool m_incomingMouseButtonRelease = false;
        bool m_incomingMouseButtonRepeat = false;
        bool m_incomingMouseScroll = false;
        bool m_incomingCursorPosition = false;
        bool m_incomingCursorEnter = false;

        std::deque<std::string> m_incomingEventLog;
        const std::size_t m_incomingEventLogSize = 100;
        unsigned short m_incomingEventCounter = 0;

        // Initialization state.
        bool m_initialized = false;
    };
}
