/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Core/Precompiled.hpp"
#include "Core/PerformanceMetrics.hpp"
using namespace Core;

PerformanceMetrics::PerformanceMetrics() = default;
PerformanceMetrics::~PerformanceMetrics() = default;

PerformanceMetrics::CreateResult PerformanceMetrics::Create()
{
    auto instance = std::unique_ptr<PerformanceMetrics>(new PerformanceMetrics());

    LOG_SUCCESS("Created performance metrics instance.");
    return Common::Success(std::move(instance));
}

void PerformanceMetrics::MarkFrameStart()
{
    m_frameStart = std::chrono::steady_clock::now();
}

void PerformanceMetrics::MarkFrameEnd()
{
    m_frameEnd = std::chrono::steady_clock::now();

    m_frameTimeAccumulated += std::chrono::duration<float>(m_frameEnd - m_frameStart).count();
    m_frameTimeAccumulations += 1;

    if(std::chrono::duration<float>(m_frameEnd - m_frameTimeUpdate).count() >= 0.1f)
    {
        m_frameTimeUpdate = m_frameEnd;
        m_frameTimeAverage = m_frameTimeAccumulated / m_frameTimeAccumulations;
        m_frameTimeAccumulated = 0.0f;
        m_frameTimeAccumulations = 0;
    }
}

float PerformanceMetrics::GetFrameTime() const
{
    return m_frameTimeAverage;
}

float PerformanceMetrics::GetFrameRate() const
{
    return 1.0f / GetFrameTime();
}
