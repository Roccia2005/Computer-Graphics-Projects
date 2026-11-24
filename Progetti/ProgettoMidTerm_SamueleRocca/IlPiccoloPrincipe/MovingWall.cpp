#include "MovingWall.h"

MovingWall::MovingWall(const glm::vec2& closedCenter, const glm::vec2& openCenter,
                       const glm::vec2& size, float travelSeconds)
    : m_closedCenter(closedCenter),
      m_openCenter(openCenter),
      m_size(size),
      m_travelSeconds(travelSeconds)
{
}

glm::vec2 MovingWall::position() const
{
    return glm::mix(m_closedCenter, m_openCenter, m_transition.progress());
}

SolidCollider MovingWall::collider() const
{
    return {position(), m_size};
}

void MovingWall::update(float dt, bool activated)
{
    m_transition.update(dt, activated, m_travelSeconds);
}

void MovingWall::reset()
{
    m_transition.reset();
}
