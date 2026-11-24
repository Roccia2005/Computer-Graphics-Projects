#pragma once
#include "Scene.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <utility>

struct MaterialBinding {
    const Shader& shader;
    void (*bindParameters)(const Shader&) = nullptr;
};

class Renderer {
    std::unordered_map<MaterialId, MaterialBinding> m_materials;
    glm::mat4 m_projection{1.0f};
    glm::ivec4 m_viewport{};
public:
    /**
     * @param materials Associazione tra materiali, shader e parametri di disegno.
     *
     * Associa a ogni materiale il programma di disegno e i suoi parametri.
     */
    explicit Renderer(std::unordered_map<MaterialId, MaterialBinding> materials)
        : m_materials(std::move(materials)) {}
    /**
     * @param framebufferWidth Larghezza del framebuffer in pixel.
     * @param framebufferHeight Altezza del framebuffer in pixel.
     * @param roomHalfSize Semidimensioni della stanza in unità mondo.
     *
     * Centra il viewport nel framebuffer e imposta una proiezione che inquadra tutta la stanza.
     */
    void setRoomViewport(int framebufferWidth, int framebufferHeight, const glm::vec2& roomHalfSize);
    /**
     * @param world Punto da proiettare in coordinate mondo.
     * @param framebuffer Dimensioni del framebuffer in pixel.
     * @param window Dimensioni logiche della finestra.
     * @return Posizione logica nella finestra, con origine in alto a sinistra.
     *
     * Proietta sul viewport OpenGL e converte in coordinate logiche dalla cima della finestra.
     */
    glm::vec2 windowPosition(glm::vec2 world, glm::ivec2 framebuffer, glm::ivec2 window) const;
    /**
     * @param scene Scena con le istanze da disegnare.
     *
     * Disegna in ordine di inserimento le istanze con un materiale registrato per questo pass.
     */
    void drawScene(const Scene& scene, SceneLayer layer = SceneLayer::World) const;
    /**
     * @return Matrice di proiezione corrente.
     *
     * Espone la proiezione ortografica calcolata per la stanza.
     */
    const glm::mat4& projectionMatrix() const { return m_projection; }
};
