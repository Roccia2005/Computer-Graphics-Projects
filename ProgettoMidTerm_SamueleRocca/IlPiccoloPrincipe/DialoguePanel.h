#pragma once
#include <glm/glm.hpp>

enum class UiAction;
struct DialogueLine;

struct DialoguePanelResult {
    UiAction action;
    glm::vec2 characterAreaTopLeft;
    glm::vec2 characterAreaBottomRight;
    glm::vec2 viewportSize;
};

namespace DialoguePanel {

/**
 * @param line Battuta e personaggio che la pronuncia.
 * @return Azione richiesta e area disponibile sopra il pannello per disegnare i personaggi.
 *
 * Mostra una battuta e rileva una nuova pressione di Invio o di Continua.
 */
DialoguePanelResult draw(const DialogueLine& line);

}
