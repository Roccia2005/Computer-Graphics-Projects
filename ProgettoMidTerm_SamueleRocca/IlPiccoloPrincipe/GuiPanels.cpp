#include "GuiPanels.h"
#include <algorithm>
#include <cmath>

namespace GuiPanels {

const ImVec4 headingColor{0.91f, 0.73f, 0.45f, 1.0f};

float panelWidth(float maximum)
{
    return std::floor(std::min(maximum, ImGui::GetMainViewport()->WorkSize.x * 0.92f));
}

bool beginPanel(const char* name, ImVec2 size, float positionY, float anchorY,
                ImGuiWindowFlags extraFlags)
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 pos{viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
                    viewport->WorkPos.y + viewport->WorkSize.y * positionY};
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, {0.5f, anchorY});
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowBgAlpha(0.95f);
    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
    if (size.y == 0.0f) extraFlags |= ImGuiWindowFlags_AlwaysAutoResize;
    return ImGui::Begin(name, nullptr, flags | extraFlags);
}

}
