#include "StarHud.h"
#include "GuiPanels.h"
#include "LevelTypes.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>

namespace {

constexpr ImGuiWindowFlags hudWindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs
        | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;

/**
 * @param remainingSeconds Tempo residuo della stanza in secondi.
 * @return Dicitura del timer in minuti e secondi, arrotondata al secondo superiore.
 */
std::string timerText(float remainingSeconds)
{
    const double seconds = std::ceil(static_cast<double>(remainingSeconds));
    char text[64];
    std::snprintf(text, sizeof(text), "Tempo %02.0f:%02.0f",
                  std::floor(seconds / 60.0), std::fmod(seconds, 60.0));
    return text;
}

/**
 * @param missing Numero di stelle ancora da raccogliere.
 * @param doorTopInWindow Centro del bordo superiore della porta in coordinate logiche della finestra.
 *
 * Mostra quante stelle mancano sopra la porta e mantiene l'avviso entro i bordi della finestra.
 */
void drawLockedExitHint(std::size_t missing, glm::vec2 doorTopInWindow)
{
    const auto& viewport = *ImGui::GetMainViewport();
    const auto padding = ImGui::GetStyle().WindowPadding;
    const std::string hint = missing == 1 ? "Manca 1 stella" : "Mancano " + std::to_string(missing) + " stelle";
    const auto size = ImGui::CalcTextSize(hint.c_str());
    const float width = size.x + 2.0f * padding.x;
    const float height = size.y + 2.0f * padding.y;
    const float x = viewport.Pos.x + doorTopInWindow.x - width * 0.5f;
    const float y = viewport.Pos.y + doorTopInWindow.y - height - padding.y;
    ImGui::SetNextWindowSize({width, height});
    ImGui::SetNextWindowPos({
        std::clamp(x, viewport.WorkPos.x, viewport.WorkPos.x + std::max(0.0f, viewport.WorkSize.x - width)),
        std::clamp(y, viewport.WorkPos.y, viewport.WorkPos.y + std::max(0.0f, viewport.WorkSize.y - height))
    });
    if (ImGui::Begin("##PortaBloccata", nullptr, hudWindowFlags)) ImGui::TextUnformatted(hint.c_str());
    ImGui::End();
}

}

void StarHud::draw(const StarProgress& stars, std::optional<float> remainingSeconds,
                   glm::vec2 doorTopInWindow, float worldTime)
{
    if (stars.total == 0 && !remainingSeconds) return;

    const auto& viewport = *ImGui::GetMainViewport();
    const auto padding = ImGui::GetStyle().WindowPadding;
    constexpr float noticeSeconds = 2.0f;
    constexpr char notice[] = "Uscita sbloccata";
    const bool showNotice = stars.unlockedAt && worldTime >= *stars.unlockedAt
        && worldTime - *stars.unlockedAt < noticeSeconds;
    const bool expiring = remainingSeconds && *remainingSeconds <= 10.0f;
    const ImVec4 alertColor{1.0f, 0.55f, 0.35f, 1.0f};
    const auto counter = stars.total > 0
        ? "Stelle " + std::to_string(stars.collected) + "/" + std::to_string(stars.total) : "";
    const auto timer = remainingSeconds ? timerText(*remainingSeconds) : "";
    struct Label {
        const char* text;
        ImVec4 color;
    };
    const Label row[] = {
        {counter.c_str(), GuiPanels::headingColor},
        {timer.c_str(), expiring ? alertColor : ImGui::GetStyleColorVec4(ImGuiCol_Text)},
        {expiring ? "In scadenza" : "", alertColor}
    };
    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    ImVec2 extent{0.0f, ImGui::GetTextLineHeight()};
    for (const Label& label : row) {
        if (*label.text) {
            if (extent.x > 0.0f) extent.x += spacing;
            extent.x += ImGui::CalcTextSize(label.text).x;
        }
    }
    if (showNotice) {
        const auto noticeSize = ImGui::CalcTextSize(notice);
        extent.x = std::max(extent.x, noticeSize.x);
        extent.y += ImGui::GetStyle().ItemSpacing.y + noticeSize.y;
    }
    ImGui::SetNextWindowSize({extent.x + 2 * padding.x, extent.y + 2 * padding.y});
    ImGui::SetNextWindowPos({viewport.WorkPos.x + padding.x, viewport.WorkPos.y + padding.y});
    if (ImGui::Begin("##Stelle", nullptr, hudWindowFlags)) {
        bool first = true;
        for (const Label& label : row) {
            if (*label.text) {
                if (!first) ImGui::SameLine();
                ImGui::TextColored(label.color, "%s", label.text);
                first = false;
            }
        }
        if (showNotice) ImGui::TextUnformatted(notice);
    }
    ImGui::End();

    if (stars.atLockedExit) {
        drawLockedExitHint(stars.total - stars.collected, doorTopInWindow);
    }
}
