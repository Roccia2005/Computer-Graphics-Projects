#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Config.h"
#include "Gui.h"
#include "GuiPanels.h"
#include "GameFlow.h"
#include <algorithm>
#include <stdexcept>

namespace {

/**
 * @param label Testo del pulsante.
 * @param selected true se la voce è selezionata da tastiera.
 * @return true se il pulsante è stato attivato.
 *
 * Evidenzia il pulsante selezionato con le frecce e rileva anche il clic.
 */
bool menuButton(const char* label, bool selected)
{
    if (selected) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    }
    const bool clicked = ImGui::Button(label, {-1.0f, 42.0f});
    if (selected) {
        ImGui::PopStyleColor();
    }
    return clicked;
}

}

Gui::Gui(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::GetIO().IniFilename = nullptr;

    ImFontAtlas& fonts = *ImGui::GetIO().Fonts;
    ImFont* font = fonts.AddFontDefault();
    if (fonts.Build()) {
        font->AddRemapChar(0x2019, '\'');
    }

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = {24.0f, 20.0f};
    style.ItemSpacing = {8.0f, 10.0f};
    style.WindowRounding = 9.0f;
    style.FrameRounding = 5.0f;
    style.Colors[ImGuiCol_WindowBg] = {0.08f, 0.09f, 0.13f, 1.0f};
    style.Colors[ImGuiCol_Border] = {0.57f, 0.43f, 0.27f, 0.75f};
    style.Colors[ImGuiCol_Button] = {0.19f, 0.23f, 0.31f, 1.0f};
    style.Colors[ImGuiCol_ButtonHovered] = {0.31f, 0.35f, 0.44f, 1.0f};
    style.Colors[ImGuiCol_ButtonActive] = {0.43f, 0.34f, 0.23f, 1.0f};

    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        throw std::runtime_error("Errore inizializzazione backend ImGui GLFW");
    }
    if (!ImGui_ImplOpenGL3_Init("#version 330 core")) {
        throw std::runtime_error("Errore inizializzazione backend ImGui OpenGL");
    }
}

void Gui::beginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

UiAction Gui::drawMenu()
{
    UiAction action = UiAction::None;
    if (GuiPanels::beginPanel("Menu principale", {GuiPanels::panelWidth(400.0f), 0.0f}, 0.48f, 0.5f)) {
        if (ImGui::IsWindowAppearing() || ImGui::IsKeyPressed(ImGuiKey_UpArrow, false)) {
            m_tutorialSelected = false;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_DownArrow, false)) {
            m_tutorialSelected = true;
        }

        const char* title = GameConfig::GAME_TITLE;
        const float availableWidth = ImGui::GetContentRegionAvail().x;
        const float titleWidth = ImGui::CalcTextSize(title).x;
        const float titleScale = std::clamp(availableWidth / titleWidth, 1.0f, 2.0f);
        ImGui::SetWindowFontScale(titleScale);
        const float margin = std::max(0.0f, (availableWidth - titleWidth * titleScale) * 0.5f);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + margin);
        ImGui::TextColored(GuiPanels::headingColor, "%s", title);
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Spacing();

        if (menuButton("Avvia storia", !m_tutorialSelected)) {
            action = UiAction::StartStory;
        }
        if (menuButton("Tutorial", m_tutorialSelected)) {
            action = UiAction::StartTutorial;
        }
        if (action == UiAction::None && ImGui::IsKeyPressed(ImGuiKey_Enter, false)) {
            action = m_tutorialSelected ? UiAction::StartTutorial : UiAction::StartStory;
        }
    }
    ImGui::End();
    return action;
}

void Gui::drawTutorialControls()
{
    if (GuiPanels::beginPanel("Comandi del tutorial", {GuiPanels::panelWidth(670.0f), 0.0f}, 0.04f, 0.0f, ImGuiWindowFlags_NoInputs)) {
        ImGui::TextColored(GuiPanels::headingColor, "%s", "Tutorial");
        ImGui::TextWrapped("Piccolo Principe   A / D movimento, W salto, S abbassati");
        ImGui::TextWrapped(u8"Volpe   Frecce laterali, Su salto, Gi\u00f9 abbassati");
        ImGui::TextWrapped("Spingi la cassa sulla pedana, abbassati nel passaggio basso e salta sopra il serpente.");
        ImGui::TextWrapped("R  Ricomincia     Esc  Menu principale");
    }
    ImGui::End();
}

void Gui::render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

Gui::~Gui()
{
    if (!ImGui::GetCurrentContext()) return;
    if (ImGui::GetIO().BackendRendererUserData) ImGui_ImplOpenGL3_Shutdown();
    if (ImGui::GetIO().BackendPlatformUserData) ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
