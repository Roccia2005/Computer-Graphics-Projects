#pragma once
#include "Shader.h"
#include "Scene.h"
#include <glm/glm.hpp>

class StarRenderer {
public:
    /**
     * @param shader Programma GLSL usato per il disegno.
     *
     * Conserva il programma GLSL che anima e disegna le stelle.
     */
    explicit StarRenderer(const Shader& shader) : m_shader(shader) {}
    /**
     * @param scene Scena che contiene le istanze delle stelle.
     * @param projection Matrice di proiezione della stanza.
     * @param timeSeconds Tempo corrente dell'animazione o della simulazione, in secondi.
     *
     * Disegna le istanze visibili del passaggio Stars usando la proiezione della stanza e il tempo
     * in secondi. Le istanze nascoste restano nel conteggio, così la fase delle stelle visibili
     * resta stabile dopo una raccolta.
     */
    void draw(const Scene& scene, const glm::mat4& projection, float timeSeconds) const;
private:
    const Shader& m_shader;
};
