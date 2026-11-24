#include "DialoguePanel.h"
#include "GuiPanels.h"
#include "GameFlow.h"
#include <algorithm>
#include <cmath>

namespace {

constexpr float dialogueAnchorY = 0.94f;

struct DialogueLayout {
    ImVec2 panelSize;
    ImVec2 padding;
    ImVec2 textArea;
    float wrapWidth;
    bool needsScroll;
};

/**
 * @param line Battuta e personaggio da mostrare.
 * @return Dimensioni del pannello e spazio disponibile per il testo.
 *
 * Riserva lo spazio per il pulsante Continua. Se la battuta supera l'altezza disponibile,
 * limita il pannello alla finestra e lascia spazio alla barra di scorrimento.
 */
DialogueLayout measureDialogue(const DialogueLine& line)
{
    const auto& style = ImGui::GetStyle();
    const float width = std::max(1.0f, GuiPanels::panelWidth(680.0f));
    const float maxHeight = std::max(1.0f, std::floor(ImGui::GetMainViewport()->WorkSize.y * dialogueAnchorY));
    const float buttonHeight = ImGui::GetFrameHeight();
    const ImVec2 padding{std::min(style.WindowPadding.x, std::floor(width * 0.1f)),
                         std::clamp(std::floor((maxHeight - buttonHeight) * 0.25f),
                                    0.0f, style.WindowPadding.y)};
    const float innerWidth = std::max(1.0f, width - 2.0f * padding.x);
    const char* end = line.text.data() + line.text.size();
    const float nameHeight = ImGui::CalcTextSize(speakerName(line.speaker), nullptr, false, innerWidth).y;
    const float lineHeight = ImGui::CalcTextSize(line.text.data(), end, false, innerWidth).y;
    const float fixedHeight = 2.0f * padding.y + style.ItemSpacing.y + buttonHeight;
    const float contentHeight = fixedHeight + (nameHeight + style.ItemSpacing.y + lineHeight);
    const bool needsScroll = contentHeight > maxHeight;
    const float height = std::min(maxHeight, contentHeight);
    const float wrapWidth = needsScroll ? std::max(1.0f, innerWidth - style.ScrollbarSize) : innerWidth;
    return {{width, height}, padding, {innerWidth, height - fixedHeight}, wrapWidth, needsScroll};
}

}

DialoguePanelResult DialoguePanel::draw(const DialogueLine& line)
{
    const DialogueLayout layout = measureDialogue(line);
    const auto& viewport = *ImGui::GetMainViewport();
    const glm::vec2 workOrigin{viewport.WorkPos.x - viewport.Pos.x, viewport.WorkPos.y - viewport.Pos.y};
    DialoguePanelResult result{UiAction::None, workOrigin, workOrigin, {viewport.Size.x, viewport.Size.y}};

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, layout.padding);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, {1.0f, 1.0f});
    if (GuiPanels::beginPanel("Dialogo", layout.panelSize, dialogueAnchorY, 1.0f)) {
        result.characterAreaBottomRight = {workOrigin.x + viewport.WorkSize.x, ImGui::GetWindowPos().y - viewport.Pos.y};
        if (!layout.needsScroll || layout.textArea.y >= 1.0f) {
            bool showText = true;
            if (layout.needsScroll) {
                ImGui::PushID(&line);
                showText = ImGui::BeginChild("Testo", layout.textArea, 0,
                                              ImGuiWindowFlags_AlwaysVerticalScrollbar);
                if (ImGui::IsWindowAppearing()) ImGui::SetScrollY(0.0f);
            }
            if (showText) {
                ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + layout.wrapWidth);
                ImGui::TextColored(GuiPanels::headingColor, "%s", speakerName(line.speaker));
                ImGui::TextUnformatted(line.text.data(), line.text.data() + line.text.size());
                ImGui::PopTextWrapPos();
            }
            if (layout.needsScroll) {
                ImGui::EndChild();
                ImGui::PopID();
            }
        }
        constexpr char next[] = "Continua (Invio)";
        const float buttonWidth = ImGui::CalcTextSize(next).x + 2.0f * ImGui::GetStyle().FramePadding.x;
        if (ImGui::Button(next, {std::min(layout.textArea.x, buttonWidth), 0.0f})
            || ImGui::IsKeyPressed(ImGuiKey_Enter, false)) {
            result.action = UiAction::AdvanceDialogue;
        }
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
    return result;
}
