#include "PressurePlate.h"
#include "LevelDefinition.h"

PressurePlate::PressurePlate(const PressurePlateDefinition& definition)
    : m_basePosition(definition.basePosition),
      m_size(definition.width, definition.height),
      m_pressDistance(definition.pressDistance)
{
}

SolidCollider PressurePlate::collider() const
{
    return colliderFor(m_pressed);
}

SolidCollider PressurePlate::releasedCollider() const
{
    return colliderFor(false);
}

float PressurePlate::update(bool hasSupport, bool releaseBlocked)
{
    if (hasSupport == m_pressed || (!hasSupport && releaseBlocked)) {
        return 0.0f;
    }

    const float previousSurfaceY = surfaceY(m_pressed);
    m_pressed = hasSupport;
    return surfaceY(m_pressed) - previousSurfaceY;
}

void PressurePlate::reset()
{
    m_pressed = false;
}

/**
 * @param pressed True per lo stato premuto, false per quello rilasciato.
 * @return Quota della superficie di appoggio.
 *
 * Calcola la quota Y della superficie in coordinate mondo per lo stato indicato da pressed.
 */
float PressurePlate::surfaceY(bool pressed) const
{
    return m_basePosition.y + m_size.y - (pressed ? m_pressDistance : 0.0f);
}

/**
 * @param pressed True per lo stato premuto, false per quello rilasciato.
 * @return Solido corrispondente allo stato richiesto.
 *
 * Costruisce il collider per lo stato richiesto, dalla base fissa alla superficie di appoggio.
 * Restituisce un SolidCollider con centro e dimensioni complete.
 */
SolidCollider PressurePlate::colliderFor(bool pressed) const
{
    const float height = surfaceY(pressed) - m_basePosition.y;
    return {
        m_basePosition + glm::vec2{0.0f, height * 0.5f},
        {m_size.x, height}
    };
}
