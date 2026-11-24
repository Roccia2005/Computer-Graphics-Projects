#pragma once
#include "MeshData.h"
#include <glad/glad.h>
class Geometry {
    GLuint m_vao=0,m_vbo=0;
    GLsizei m_vertexCount=0;
    GLenum m_primitive=GL_TRIANGLES;
public:
    /**
     * @param mesh Geometria con posizioni e colori dei vertici.
     *
     * Carica nella GPU i vertici di mesh e configura il VAO per il formato x, y, r, g, b. Memorizza
     * la primitiva e il numero di vertici da disegnare.
     */
    explicit Geometry(const MeshData& mesh);
    /**
     * Rilascia il VBO e il VAO posseduti dalla geometria. Il contesto OpenGL deve essere ancora
     * attivo.
     */
    ~Geometry();
    /**
     * Vieta la costruzione per copia per mantenere un unico proprietario delle risorse.
     */
    Geometry(const Geometry&)=delete;
    /**
     * @return Nessun valore ottenibile, poiché l'operatore è eliminato.
     *
     * Vieta l'assegnazione per copia per mantenere un unico proprietario delle risorse.
     */
    Geometry& operator=(const Geometry&)=delete;
    /**
     * Associa il VAO ed emette una draw call con la primitiva e il numero di vertici della mesh. Il
     * renderer deve avere già impostato programma e uniform.
     */
    void draw() const;
};
