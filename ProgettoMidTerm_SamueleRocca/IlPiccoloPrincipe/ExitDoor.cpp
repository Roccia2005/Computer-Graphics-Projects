#include "ExitDoor.h"

ExitDoor::ExitDoor(const glm::vec2& basePosition)
{
    constexpr glm::vec2 size{GameConfig::DOOR_WIDTH, GameConfig::DOOR_HEIGHT};
    const glm::vec2 center = basePosition + glm::vec2{0.0f, size.y * 0.5f};
    m_area = makeAABB(center, size);
}

bool ExitDoor::overlaps(const AABB& body) const
{
    return intersects(body, m_area);
}

void ExitDoor::update(float dt, bool shouldOpen)
{
    m_transition.update(dt, shouldOpen, GameConfig::DOOR_OPENING_SECONDS);
}

void ExitDoor::reset()
{
    m_transition.reset();
}
