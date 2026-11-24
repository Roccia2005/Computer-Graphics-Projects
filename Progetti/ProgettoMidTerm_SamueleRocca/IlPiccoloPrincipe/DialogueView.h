#pragma once
#include "DialogueSpeaker.h"
#include "Scene.h"
#include <array>
#include <optional>

struct DialoguePanelResult;
namespace DecorativeShapes { struct CharacterMeshes; }

class DialogueView {
public:
    /**
     * Crea le figure del Principe e della Volpe nella scena del dialogo.
     */
    DialogueView();
    /**
     * @param line Battuta corrente, oppure nullptr fuori dai dialoghi.
     *
     * Rileva un cambio di battuta e azzera il tempo di inizio del gesto.
     */
    void observeLine(const DialogueLine* line);
    /**
     * @param panel Area del dialogo calcolata dall'interfaccia.
     * @param framebufferSize Dimensioni del framebuffer in pixel.
     * @param now Tempo corrente in secondi.
     * @return true se le figure sono state posizionate e possono essere disegnate.
     *
     * Adatta le figure al pannello e anima la testa del personaggio che parla.
     */
    bool updatePose(const DialoguePanelResult& panel, glm::vec2 framebufferSize, double now);
    /**
     * @return Riferimento in sola lettura alla scena.
     *
     * Espone le geometrie e le istanze predisposte per il disegno.
     */
    const Scene& scene() const { return m_scene; }

private:
    struct Figure {
        DialogueSpeaker speaker;
        std::size_t bodyInstanceIndex;
        std::size_t headInstanceIndex;
        std::optional<std::size_t> foregroundInstanceIndex;
    };
    Figure addFigure(DialogueSpeaker speaker, const DecorativeShapes::CharacterMeshes& parts);
    void poseFigure(const Figure& figure, glm::vec2 center, float scale,
                    const glm::mat4& headMotion);

    Scene m_scene;
    std::array<Figure, 2> m_figures;
    const DialogueLine* m_line = nullptr;
    std::optional<double> m_gestureStart;
};
