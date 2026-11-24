#pragma once
#include <glad/glad.h>
#include "Shader.h"

class BackgroundRenderer
{
public:
    /**
     * @param shader Programma GLSL usato per il disegno.
     *
     * Carica nella GPU un rettangolo in coordinate NDC che copre tutto il viewport. Conserva un
     * riferimento allo shader, che deve restare in vita per tutta la durata del renderer.
     */
    explicit BackgroundRenderer(const Shader& shader);
    /**
     * Rilascia il VAO e il VBO dello sfondo. Il contesto OpenGL deve essere ancora attivo.
     */
    ~BackgroundRenderer();
    /**
     * Vieta la costruzione per copia per mantenere un unico proprietario delle risorse.
     */
    BackgroundRenderer(const BackgroundRenderer&) = delete;
    /**
     * @return Nessun valore ottenibile, poiché l'operatore è eliminato.
     *
     * Vieta l'assegnazione per copia per mantenere un unico proprietario delle risorse.
     */
    BackgroundRenderer& operator=(const BackgroundRenderer&) = delete;
    /**
     * @param timeSeconds Tempo corrente dell'animazione o della simulazione, in secondi.
     *
     * Disegna lo sfondo animato nel viewport corrente.
     */
    void draw(float timeSeconds) const;

private:
    const Shader& m_shader;
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
};
