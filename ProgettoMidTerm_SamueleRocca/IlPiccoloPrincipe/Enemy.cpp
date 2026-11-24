#include "Enemy.h"
#include "LevelDefinition.h"
#include <cmath>

Enemy::Enemy(const EnemyDefinition& definition)
    : m_startCenter(definition.startCenter),
      m_endCenter(definition.endCenter),
      m_size(definition.size),
      m_speed(definition.speed)
{
}

void Enemy::update(float dt)
{
    const double pathLength = glm::length(glm::dvec2(m_endCenter) - glm::dvec2(m_startCenter));
    if (dt <= 0.0f || pathLength == 0.0 || m_speed <= 0.0f) {
        return;
    }

    const double advance = static_cast<double>(dt) * m_speed / pathLength;
    m_pathPhase = std::fmod(m_pathPhase + advance, 2.0);
}

AABB Enemy::bounds() const
{
    const float progress = static_cast<float>(1.0 - std::abs(1.0 - m_pathPhase));
    return makeAABB(glm::mix(m_startCenter, m_endCenter, progress), m_size);
}

void Enemy::reset()
{
    m_pathPhase = 0.0;
}
