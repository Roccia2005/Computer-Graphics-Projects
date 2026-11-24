#include "SceneCollider.h"
#include "Pushable.h"

SolidCollider SceneCollider::collider() const
{
    return m_pushable ? m_pushable->collider() : m_fixed;
}
