/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "../Precompiled.hpp"
#include "Editor/Modules/GameInstanceEditor.hpp"
#include <Game/GameFramework.hpp>
#include <Game/GameInstance.hpp>
#include <Game/GameState.hpp>
using namespace Editor;

GameInstanceEditor::GameInstanceEditor()
{
    m_receivers.gameStateChanged.Bind<GameInstanceEditor, &GameInstanceEditor::OnGameStateChanged>(this);
    m_receivers.tickRequested.Bind<GameInstanceEditor, &GameInstanceEditor::OnTickRequested>(this);
    m_receivers.tickProcessed.Bind<GameInstanceEditor, &GameInstanceEditor::OnTickProcessed>(this);
}

GameInstanceEditor::~GameInstanceEditor() = default;

GameInstanceEditor::CreateResult GameInstanceEditor::Create(const CreateFromParams& params)
{
    LOG("Creating game instance editor...");
    LOG_SCOPED_INDENT();

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(params.services != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Acquire game framework service.
    Game::GameFramework* gameFramework = params.services->GetGameFramework();

    // Create instance.
    auto instance = std::unique_ptr<GameInstanceEditor>(new GameInstanceEditor());

    // Subscribe to game framework events.
    instance->m_receivers.gameStateChanged.Subscribe(gameFramework->events.gameStateChanged);
    instance->m_receivers.tickRequested.Subscribe(gameFramework->events.tickRequested);
    instance->m_receivers.tickProcessed.Subscribe(gameFramework->events.tickProcessed);

    // Set histogram size.
    instance->m_tickTimeHistogram.resize(100, 0.0f);

    // Success!
    return Common::Success(std::move(instance));
}

void GameInstanceEditor::Update(float timeDelta)
{
    // Do not create a window if game instance reference is not set.
    if(!m_gameInstance)
    {
        mainWindowOpen = false;
        return;
    }

    // Show game instance window.
    if(mainWindowOpen)
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(200.0f, 0.0f), ImVec2(FLT_MAX, FLT_MAX));

        if(ImGui::Begin("Game Framework", &mainWindowOpen, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if(ImGui::CollapsingHeader("Core"))
            {
                if(m_tickTimer && ImGui::TreeNode("Tick Timer"))
                {
                    // Show tick controls.
                    float currentTickTime = m_tickTimer->GetTickSeconds();
                    float currentTickRate = 1.0f / currentTickTime;
                    ImGui::BulletText("Tick time: %fs (%.1f tick rate)",
                        currentTickTime, currentTickRate);

                    ImGui::SliderFloat("##TickRateSlider", &m_tickRateSlider,
                        1.0f, 100.0f, "%.1f ticks(s) per second", ImGuiSliderFlags_Logarithmic);

                    ImGui::SameLine();
                    if(ImGui::Button("Apply##TickTimeApply"))
                    {
                        m_tickTimer->SetTickSeconds(1.0f / m_tickRateSlider);
                    }

                    // Show tick histogram.
                    ImGui::BulletText("Tick time histogram:");
                    ImGui::PlotHistogram("##TickTimeHistogram",
                        &m_tickTimeHistogram[0], (int)m_tickTimeHistogram.size(),
                        0, "", FLT_MAX, FLT_MAX, ImVec2(0, 100));

                    // Process histogram statistics.
                    int tickTimeValues = 0;
                    float tickTimeMinimum = FLT_MAX;
                    float tickTimeMaximum = 0.0f;
                    float tickTimeAverage = 0.0f;

                    for(float tickTime : m_tickTimeHistogram)
                    {
                        if(tickTime == 0.0f)
                            continue;

                        tickTimeValues += 1;
                        tickTimeMinimum = std::min(tickTime, tickTimeMinimum);
                        tickTimeMaximum = std::max(tickTime, tickTimeMaximum);
                        tickTimeAverage += tickTime;
                    }

                    if(tickTimeMinimum == FLT_MAX)
                        tickTimeMinimum = 0.0f;

                    if(tickTimeValues != 0)
                    {
                        tickTimeAverage /= tickTimeValues;
                    }

                    // Print histogram statistics.
                    ImGui::SameLine();
                    ImGui::BeginGroup();
                    {
                        ImGui::Text("Min: %0.3f", tickTimeMinimum);
                        ImGui::Text("Max: %0.3f", tickTimeMaximum);
                        ImGui::Text("Avg: %0.3f", tickTimeAverage);

                        ImGui::PushID("TickTimeHistogramToggle");
                        if(ImGui::Button(m_tickTimeHistogramPaused ? "Resume" : "Pause"))
                        {
                            m_tickTimeHistogramPaused = !m_tickTimeHistogramPaused;
                        }
                        ImGui::PopID();
                    }
                    ImGui::EndGroup();

                    // Update time delay slider.
                    ImGui::BulletText("Update time delay: %0.3fs", m_updateDelayValue);
                    ImGui::SliderFloat("##UpdateDelaySlider", &m_updateDelaySlider,
                        0.0f, 1.0f, "%0.3fs delay", ImGuiSliderFlags_Logarithmic);

                    ImGui::SameLine();
                    if(ImGui::Button("Apply##UpdateDelayApply"))
                    {
                        m_updateDelayValue = m_updateDelaySlider;
                    }

                    if(m_updateDelayValue > 0.0f)
                    {
                        int sleepTime = (int)(m_updateDelayValue * 1000.0f);
                        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
                    }

                    // Update time noise slider.
                    ImGui::BulletText("Update time noise: %0.3fs", m_updateNoiseValue);
                    ImGui::SliderFloat("##UpdateNoiseSlider", &m_updateNoiseSlider,
                        0.0f, 1.0f, "%0.3fs noise", ImGuiSliderFlags_Logarithmic);

                    ImGui::SameLine();
                    if(ImGui::Button("Apply##UpdateNoiseApply"))
                    {
                        m_updateNoiseValue = m_updateNoiseSlider;
                    }

                    if(m_updateNoiseValue > 0.0f)
                    {
                        int sleepTime = std::rand() % (int)(m_updateNoiseValue * 1000.0f);
                        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
                    }

                    // Update time freeze slider.
                    ImGui::BulletText("Update time freeze:");
                    ImGui::SliderFloat("##UpdateFreezeSlider", &m_updateFreezeSlider,
                        0.1f, 10.0f, "%.1fs freeze", ImGuiSliderFlags_Logarithmic);

                    ImGui::SameLine();
                    if(ImGui::Button("Freeze"))
                    {
                        int sleepTime = (int)(m_updateFreezeSlider * 1000.0f);
                        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
                    }

                    ImGui::TreePop();
                }

                if(m_gameInstance && ImGui::TreeNode("Game Instance"))
                {
                    ImGui::TreePop();
                }
            }
        }

        ImGui::End();
    }
}

void GameInstanceEditor::OnGameStateChanged(const std::shared_ptr<Game::GameState>& gameState)
{
    // Save references from new game state.
    m_tickTimer = gameState ? gameState->GetTickTimer() : nullptr;
    m_gameInstance = gameState ? gameState->GetGameInstance() : nullptr;

    // Update editor state.
    if(m_tickTimer)
    {
        // Update tick time slider value.
        m_tickRateSlider = 1.0f / m_tickTimer->GetTickSeconds();

        // Clear tick time histogram.
        for(auto& tickTime : m_tickTimeHistogram)
        {
            tickTime = 0.0f;
        }
    }
}

void GameInstanceEditor::OnTickRequested()
{
    // Do not process histogram data if paused.
    if(m_tickTimeHistogramPaused)
        return;

    // Rotate tick time histogram array to the right.
    if(m_tickTimeHistogram.size() >= 2)
    {
        std::rotate(m_tickTimeHistogram.begin(), m_tickTimeHistogram.begin() + 1, m_tickTimeHistogram.end());
    }

    // Reset first value that will accumulate new tick time values.
    if(!m_tickTimeHistogram.empty())
    {
        m_tickTimeHistogram.back() = 0.0f;
    }
}

void GameInstanceEditor::OnTickProcessed(float tickTime)
{
    // Do not process histogram data if paused.
    if(m_tickTimeHistogramPaused)
        return;

    // Accumulate new tick time.
    if(!m_tickTimeHistogram.empty())
    {
        m_tickTimeHistogram.back() += tickTime;
    }
}
