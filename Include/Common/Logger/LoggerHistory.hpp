/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <deque>
#include "Common/Logger/LoggerOutput.hpp"
#include "Common/Logger/LoggerMessage.hpp"

/*
    Logger History
*/

namespace Logger
{
    class History final : public Output
    {
    public:
        struct MessageEntry
        {
            Logger::Severity::Type severity;
            std::string text;
        };

        using MessageList = std::deque<MessageEntry>;

    public:
        History();
        ~History();

        bool Initialize() const override;
        void Write(const Logger::Message& message, const Logger::SinkContext& context) override;
        const MessageList& GetMessages() const;

    private:
        MessageList m_messages;
    };
}
