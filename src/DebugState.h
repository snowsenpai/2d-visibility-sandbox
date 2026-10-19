#pragma once

// Plain data shared between the UI (writes) and debug drawing / geometry (reads).
// No SFML window, no ImGui includes.
struct DebugState
{
    bool  showSegments = true;
    bool  showRays = true;
    bool  showHitPoints = true;
    int   rayCount = 64;
    float fovDegrees = 90.f;
    // Add toggles here as each checkpoint needs them.
};