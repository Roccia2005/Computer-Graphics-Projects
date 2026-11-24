#include "StarRenderer.h"
#include "LevelTypes.h"

void StarRenderer::draw(const Scene& scene, const glm::mat4& projection,
                        float timeSeconds) const
{
    m_shader.use();
    m_shader.setMat4("projection", projection);
    m_shader.setFloat("uTime", timeSeconds);
    m_shader.setFloat("uAmplitude", GameConfig::STAR_BOB_AMPLITUDE);
    m_shader.setFloat("uFrequency", GameConfig::STAR_BOB_FREQUENCY);

    std::size_t starIndex = 0;
    for (const GeometryInstance& instance : scene.instances()) {
        if (instance.material != MaterialId::Star) continue;
        const float phase = GameConfig::starPhase(starIndex++);
        if (!instance.visible) continue;
        m_shader.setFloat("uPhase", phase);
        m_shader.setMat4("model", instance.modelMatrix);
        scene.geometry(instance.geometryIndex).draw();
    }
    glBindVertexArray(0);
}
