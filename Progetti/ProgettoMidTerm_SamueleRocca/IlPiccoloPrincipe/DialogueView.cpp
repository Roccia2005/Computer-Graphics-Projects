#include "DialogueView.h"
#include "Config.h"
#include "DecorativeShapes.h"
#include "DialoguePanel.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

namespace {

struct HeadGesture {
    glm::vec2 neckPivot;
    float headOffsetY;
    float headAngleDegrees;
    double durationSeconds = 0.45;
};

/**
 * @param speaker Personaggio che pronuncia la battuta.
 * @return Parametri del gesto del personaggio.
 *
 * Seleziona durata, rotazione e punto di rotazione della testa del personaggio.
 */
const HeadGesture& headGesture(DialogueSpeaker speaker)
{
    static constexpr std::array gestures{
        HeadGesture{{0.0f, 0.0f}, -0.04f, 0.0f},
        HeadGesture{{-0.05f, 0.06f}, 0.0f, 8.0f}
    };
    static_assert(gestures.size() == static_cast<std::size_t>(DialogueSpeaker::Count));
    return gestures.at(static_cast<std::size_t>(speaker));
}

const glm::vec2 standingSize = glm::vec2(GameConfig::PLAYER_WIDTH, GameConfig::PLAYER_HEIGHT)
                             * GameConfig::PLAYER_RENDER_SCALE;

struct DialogueLayout {
    std::array<glm::vec2, 2> figureCenters;
    float scale;
};

/**
 * @return Margine necessario per il gesto più ampio dei personaggi.
 *
 * Somma lo spostamento verticale e quello dovuto alla rotazione attorno al collo.
 */
float calculateMaxHeadDisplacement()
{
    float maximum = 0.0f;
    for (const auto speaker : {DialogueSpeaker::Prince, DialogueSpeaker::Fox}) {
        const auto& gesture = headGesture(speaker);
        const float radius = glm::length(standingSize * (0.5f + glm::abs(gesture.neckPivot)));
        const float angle = glm::radians(std::abs(gesture.headAngleDegrees));
        const float rotationTravel = 2.0f * radius * std::sin(angle * 0.5f);
        const float verticalTravel = std::abs(gesture.headOffsetY) * standingSize.y;
        maximum = std::max(maximum, verticalTravel + rotationTravel);
    }
    return maximum;
}

/**
 * @param panel Area del dialogo calcolata dall'interfaccia.
 * @param framebufferSize Dimensioni del framebuffer in pixel.
 * @return Centri e scala delle figure, oppure std::nullopt se manca lo spazio per disegnarle.
 *
 * Dispone le figure ai lati del centro del pannello lasciando spazio per il gesto della testa.
 */
std::optional<DialogueLayout> calculateLayout(const DialoguePanelResult& panel, glm::vec2 framebufferSize)
{
    if (panel.viewportSize.x <= 0.0f || panel.viewportSize.y <= 0.0f) return std::nullopt;
    const auto pixelRatio = framebufferSize / panel.viewportSize;
    const auto available = (panel.characterAreaBottomRight - panel.characterAreaTopLeft) * pixelRatio;
    if (available.x <= 0.0f || available.y <= 0.0f) return std::nullopt;

    const auto panelCenter = (panel.characterAreaTopLeft + panel.characterAreaBottomRight) * 0.5f;
    auto center = panelCenter * pixelRatio - framebufferSize * 0.5f;
    center.y = -center.y;

    static const float maxHeadDisplacement = calculateMaxHeadDisplacement();
    const float figureWidth = standingSize.x + 2.0f * maxHeadDisplacement;
    const float centerDistance = figureWidth + 0.25f * standingSize.x;
    const float scale = 0.88f * std::min(available.x / (centerDistance + figureWidth),
                                         available.y / (standingSize.y + maxHeadDisplacement));
    center.y -= maxHeadDisplacement * scale * 0.5f;
    const glm::vec2 horizontalOffset(centerDistance * 0.5f * scale, 0.0f);
    return DialogueLayout{{center - horizontalOffset, center + horizontalOffset}, scale};
}

/**
 * @param elapsed Tempo trascorso dall'inizio del gesto, in secondi.
 * @param duration Durata complessiva in secondi.
 * @return Intensità tra zero e uno, nulla fuori dalla durata del gesto.
 *
 * Fa crescere e poi diminuire il gesto durante la battuta.
 */
float gestureIntensity(double elapsed, double duration)
{
    if (elapsed <= 0.0 || elapsed >= duration) return 0.0f;
    const double wave = std::sin(glm::pi<double>() * elapsed / duration);
    return static_cast<float>(wave * wave);
}

/**
 * @param gesture Parametri del movimento della testa.
 * @param intensity Intensità corrente del gesto, tra zero e uno.
 * @return Matrice di trasformazione della testa.
 *
 * Combina rotazione attorno al collo e spostamento verticale della testa.
 */
glm::mat4 headMotion(const HeadGesture& gesture, float intensity)
{
    const glm::vec3 pivot(gesture.neckPivot * standingSize, 0.0f);
    const float offset = gesture.headOffsetY * standingSize.y * intensity;
    auto motion = glm::translate(glm::mat4(1.0f), pivot + glm::vec3(0.0f, offset, 0.0f));
    motion = glm::rotate(motion, glm::radians(gesture.headAngleDegrees) * intensity,
                         {0.0f, 0.0f, 1.0f});
    return glm::translate(motion, -pivot);
}

}

DialogueView::DialogueView()
    : m_figures{addFigure(DialogueSpeaker::Prince, DecorativeShapes::littlePrinceParts()),
                addFigure(DialogueSpeaker::Fox, DecorativeShapes::foxParts())}
{
}

/**
 * @param speaker Personaggio che pronuncia la battuta.
 * @param parts Mesh dei diversi strati della figura.
 * @return Riferimenti alle istanze che compongono il personaggio.
 *
 * Crea le istanze di corpo e testa e aggiunge i dettagli in primo piano se presenti.
 */
DialogueView::Figure DialogueView::addFigure(DialogueSpeaker speaker,
                                             const DecorativeShapes::CharacterMeshes& parts)
{
    const auto add = [this](const MeshData& mesh) {
        return m_scene.addInstance(m_scene.addGeometry(mesh));
    };
    Figure figure{speaker, add(parts.body), add(parts.head), std::nullopt};
    if (!parts.foreground.vertices.empty()) figure.foregroundInstanceIndex = add(parts.foreground);
    return figure;
}

void DialogueView::observeLine(const DialogueLine* line)
{
    if (m_line != line) {
        m_line = line;
        m_gestureStart.reset();
    }
}

/**
 * @param figure Figura del dialogo da posizionare.
 * @param center Centro nel sistema di coordinate di destinazione.
 * @param scale Fattore di scala uniforme.
 * @param headMotion Trasformazione aggiuntiva da applicare alla testa.
 *
 * Posiziona le parti della figura e applica il gesto alla testa.
 */
void DialogueView::poseFigure(const Figure& figure, glm::vec2 center, float scale,
                              const glm::mat4& headMotion)
{
    auto placement = glm::translate(glm::mat4(1.0f), glm::vec3(center, 0.0f));
    placement = glm::scale(placement, glm::vec3(scale, scale, 1.0f));
    const glm::vec3 proportions(standingSize, 1.0f);
    const auto base = glm::scale(placement, proportions);
    m_scene.instance(figure.bodyInstanceIndex).modelMatrix = base;
    m_scene.instance(figure.headInstanceIndex).modelMatrix = glm::scale(placement * headMotion, proportions);
    if (figure.foregroundInstanceIndex) {
        m_scene.instance(*figure.foregroundInstanceIndex).modelMatrix = base;
    }
}

bool DialogueView::updatePose(const DialoguePanelResult& panel, glm::vec2 framebufferSize, double now)
{
    if (!m_line) return false;
    const auto layout = calculateLayout(panel, framebufferSize);
    if (!layout) return false;

    if (!m_gestureStart) m_gestureStart = now;
    const auto& gesture = headGesture(m_line->speaker);
    const float intensity = gestureIntensity(now - *m_gestureStart, gesture.durationSeconds);
    const auto motion = headMotion(gesture, intensity);
    for (std::size_t i = 0; i < m_figures.size(); ++i) {
        const Figure& figure = m_figures[i];
        poseFigure(figure, layout->figureCenters[i], layout->scale,
                   figure.speaker == m_line->speaker ? motion : glm::mat4(1.0f));
    }
    return true;
}
