#pragma once
#include <imgui.h>

namespace GuiPanels {

extern const ImVec4 headingColor;

/**
 * @param maximum Larghezza massima del pannello in coordinate ImGui.
 * @return Larghezza disponibile arrotondata per difetto.
 *
 * Limita la larghezza del pannello al 92% dello spazio disponibile.
 */
float panelWidth(float maximum);

/**
 * @param name Nome identificativo.
 * @param size Dimensioni del pannello in coordinate ImGui.
 * @param positionY Posizione verticale relativa all'area di lavoro.
 * @param anchorY Frazione dell'altezza del pannello usata come punto di ancoraggio.
 * @param extraFlags Opzioni ImGui aggiuntive per la finestra.
 * @return true se ImGui richiede di disegnare il contenuto.
 *
 * Posiziona il pannello al centro in orizzontale e apre la finestra ImGui.
 */
bool beginPanel(const char* name, ImVec2 size, float positionY, float anchorY,
                ImGuiWindowFlags extraFlags = 0);

}
