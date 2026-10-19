#include "DebugPanels.h"
#include "DebugState.h"

#include <imgui.h>

void drawVisibilityPanel(DebugState& s)
{
    ImGui::Begin("Visibility Debug");

    ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
    ImGui::Separator();

    ImGui::Checkbox("Show segments", &s.showSegments);
    ImGui::Checkbox("Show rays", &s.showRays);
    ImGui::Checkbox("Show hit points", &s.showHitPoints);

    ImGui::SliderInt("Ray count", &s.rayCount, 1, 720);
    ImGui::SliderFloat("FOV (deg)", &s.fovDegrees, 1.f, 360.f);

    ImGui::End();
}