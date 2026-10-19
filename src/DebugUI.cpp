#include "DebugUI.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include <stdexcept>

DebugUI::DebugUI(sf::RenderWindow& window)
    : m_window(window)
{
    if (!ImGui::SFML::Init(m_window))
        throw std::runtime_error("ImGui::SFML::Init failed");
}

DebugUI::~DebugUI()
{
    ImGui::SFML::Shutdown(m_window);
}

void DebugUI::processEvent(const sf::Event& event)
{
    ImGui::SFML::ProcessEvent(m_window, event);
}

void DebugUI::beginFrame(sf::Time dt)
{
    ImGui::SFML::Update(m_window, dt);
}

void DebugUI::render()
{
    ImGui::SFML::Render(m_window);
}

bool DebugUI::wantsMouse() const
{
    return ImGui::GetIO().WantCaptureMouse;
}

bool DebugUI::wantsKeyboard() const
{
    return ImGui::GetIO().WantCaptureKeyboard;
}