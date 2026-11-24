#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

void Renderer::setRoomViewport(int width, int height, const glm::vec2& roomHalfSize)
{
    if (roomHalfSize.x <= 0.0f || roomHalfSize.y <= 0.0f) {
        return;
    }

    const double roomWidth = 2.0 * roomHalfSize.x;
    const double roomHeight = 2.0 * roomHalfSize.y;
    const double scale = std::min(width / roomWidth, height / roomHeight);
    const int viewportWidth = std::max(1, static_cast<int>(std::lround(roomWidth * scale)));
    const int viewportHeight = std::max(1, static_cast<int>(std::lround(roomHeight * scale)));
    m_viewport = {(width - viewportWidth) / 2, (height - viewportHeight) / 2,
                  viewportWidth, viewportHeight};
    glViewport(m_viewport.x, m_viewport.y, m_viewport.z, m_viewport.w);

    m_projection = glm::ortho(-roomHalfSize.x, roomHalfSize.x,
                              -roomHalfSize.y, roomHalfSize.y, -1.0f, 1.0f);
}

glm::vec2 Renderer::windowPosition(glm::vec2 world, glm::ivec2 framebuffer, glm::ivec2 window) const
{
    const glm::vec4 clip = m_projection * glm::vec4(world, 0.0f, 1.0f);
    const glm::vec2 unit = (glm::vec2(clip) / clip.w + 1.0f) * 0.5f;
    glm::vec2 pixel = glm::vec2(m_viewport) + unit * glm::vec2(m_viewport.z, m_viewport.w);
    pixel.y = static_cast<float>(framebuffer.y) - pixel.y;
    return pixel * glm::vec2(window) / glm::vec2(framebuffer);
}

void Renderer::drawScene(const Scene& scene, SceneLayer layer) const
{
    const MaterialBinding* active = nullptr;
    for (const GeometryInstance& instance : scene.instances()) {
        if (!instance.visible || instance.layer != layer) continue;
        const auto it = m_materials.find(instance.material);
        if (it == m_materials.end()) continue;

        const MaterialBinding& material = it->second;
        if (active != &material) {
            if (!active || &active->shader != &material.shader) {
                material.shader.use();
                material.shader.setMat4("projection", m_projection);
            }
            if (material.bindParameters) material.bindParameters(material.shader);
            active = &material;
        }
        material.shader.setMat4("model", instance.modelMatrix);
        scene.geometry(instance.geometryIndex).draw();
    }
    glBindVertexArray(0);
}
