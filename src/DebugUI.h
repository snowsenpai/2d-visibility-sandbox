#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

// Owns the ImGui-SFML lifecycle only. It knows nothing about panels or game data.
class DebugUI
{
public:
    explicit DebugUI(sf::RenderWindow& window);
    ~DebugUI();

    DebugUI(const DebugUI&) = delete;
    DebugUI& operator=(const DebugUI&) = delete;

    void processEvent(const sf::Event& event);
    void beginFrame(sf::Time dt);   // must run every frame before any ImGui:: widget call
    void render();                  // after world drawing, before window.display()

    bool wantsMouse() const;
    bool wantsKeyboard() const;

private:
    sf::RenderWindow& m_window;
};