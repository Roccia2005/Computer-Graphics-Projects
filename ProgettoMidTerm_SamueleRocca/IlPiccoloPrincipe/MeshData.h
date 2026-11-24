#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
struct MeshData {
    static constexpr int positionComponents = 2;
    static constexpr int colorComponents = 3;
    static constexpr int floatsPerVertex = positionComponents + colorComponents;

    std::vector<float> vertices;
    GLenum primitive = GL_TRIANGLES;

    /**
     * @param p Posizione del vertice nel piano locale.
     * @param color Colore RGB dei vertici.
     *
     * Accoda la posizione XY del vertice seguita dalle tre componenti del colore.
     */
    void addVertex(const glm::vec2& p, const glm::vec3& color)
    {
        vertices.insert(vertices.end(), {p.x, p.y, color.r, color.g, color.b});
    }

    /**
     * @param a Primo vertice.
     * @param b Secondo vertice.
     * @param c Terzo vertice.
     * @param color Colore RGB dei vertici.
     *
     * Accoda i tre vertici del triangolo nello stesso ordine in cui sono forniti.
     */
    void addTriangle(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c,
                     const glm::vec3& color)
    {
        addVertex(a, color);
        addVertex(b, color);
        addVertex(c, color);
    }
};
