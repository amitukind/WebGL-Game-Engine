/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <deque>
#include "Logger/Output.hpp"
#include "Logger/Message.hpp"

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

        void Write(const Logger::Message& message, const Logger::SinkContext& context) override;
        const MessageList& GetMessages() const;

    private:
        MessageList m_messages;
    };
}
