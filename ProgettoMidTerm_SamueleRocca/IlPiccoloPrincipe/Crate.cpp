#include "Crate.h"
#include "SceneCollider.h"
#include "LevelDefinition.h"
#include <algorithm>
#include <cmath>

Crate::Crate(const CrateDefinition& definition)
    : m_spawnCenter(definition.basePosition + glm::vec2{0.0f, definition.size.y * 0.5f}),
      m_position(m_spawnCenter),
      m_size(definition.size)
{
}

SolidCollider Crate::collider() const
{
    return {m_position, m_size};
}

AABB Crate::bounds() const
{
    return makeAABB(m_position, m_size);
}

void Crate::fall(float dt, float floorY, const std::vector<SceneCollider>& scene)
{
    if (dt <= 0.0f) {
        return;
    }
	const AABB box = bounds();
    const float bottom = box.center.y - box.halfSize.y;
    float supportY = floorY;
    for (const SceneCollider& contact : scene) {
        if (contact.pushable() == this) {
            continue;
        }
        const float top = contact.bounds().center.y + contact.bounds().halfSize.y;
        if (top <= bottom + contactTolerance &&
            overlapsHorizontally(box, contact.bounds(), 0.0f)) {
            supportY = std::max(supportY, top);
        }
    }

    m_velocityY += GameConfig::GRAVITY * dt;
    const float nextBottom = bottom + m_velocityY * dt;
    m_onGround = nextBottom <= supportY;
    m_position.y = std::max(nextBottom, supportY) + box.halfSize.y;
    if (m_onGround) {
        m_velocityY = 0.0f;
    }
}

void Crate::push(float distance, float roomHalfWidth, const std::vector<SceneCollider>& colliders,
                 const AABB* blockingPlayer)
{
    const AABB box = bounds();
    const float limit = roomHalfWidth - box.halfSize.x;
    const float pushDistance = distance * GameConfig::CRATE_PUSH_SPEED_FACTOR;
    const float target = std::clamp(m_position.x + pushDistance, -limit, limit);
    const float roomLimitedDx = target - m_position.x;

    float allowedDx = allowedHorizontalMove(box, roomLimitedDx, colliders, blockingPlayer);
    const bool hitObstacle = std::abs(allowedDx) < std::abs(roomLimitedDx);
    if (m_onGround && hitObstacle) {
        allowedDx = tryStepUp(box, roomLimitedDx, allowedDx, colliders, blockingPlayer);
    }
    m_position.x += allowedDx;
}

/**
 * @param body Rettangolo della cassa alla quota da verificare.
 * @param roomLimitedDx Spostamento orizzontale con segno già limitato dai confini della stanza.
 * @param colliders Solidi coinvolti nel passo di simulazione.
 * @param blockingPlayer Ingombro del compagno che può bloccare la spinta, oppure nullptr.
 * @return Spostamento orizzontale con segno consentito alla quota indicata.
 *
 * Se il percorso è libero, restituisce roomLimitedDx. Se la cassa incontra il compagno
 * o un solido lungo la spinta, restituisce lo spostamento con segno fino al primo
 * contatto. Se è già a contatto e la spinta è diretta verso l'ostacolo, restituisce zero.
 */
float Crate::allowedHorizontalMove(const AABB& body, float roomLimitedDx,
                                   const std::vector<SceneCollider>& colliders, const AABB* blockingPlayer) const
{
    float allowedDx = roomLimitedDx;
    if (blockingPlayer) {
        allowedDx = horizontalSweepLimit(body, allowedDx, *blockingPlayer, PenetrationResponse::IgnoreBeyondTolerance);
    }
    for (const SceneCollider& contact : colliders) {
        if (contact.pushable() != this) {
            allowedDx = horizontalSweepLimit(body, allowedDx, contact.bounds(), PenetrationResponse::IgnoreBeyondTolerance);
        }
    }
    return allowedDx;
}

/**
 * @param box Rettangolo della cassa prima del tentativo di salita.
 * @param roomLimitedDx Spostamento orizzontale con segno da riprovare alla quota rialzata.
 * @param groundDx Spostamento orizzontale con segno consentito alla quota di partenza.
 * @param colliders Solidi coinvolti nel passo di simulazione.
 * @param blockingPlayer Ingombro del compagno che può ostacolare la salita o la spinta, oppure nullptr.
 * @return Spostamento orizzontale da applicare, pari a groundDx se la salita fallisce.
 *
 * Se lo spazio sopra la cassa è libero e il rialzo permette di avanzare di più,
 * solleva la cassa e aggiorna il suo stato di appoggio.
 */
float Crate::tryStepUp(const AABB& box, float roomLimitedDx, float groundDx,
                       const std::vector<SceneCollider>& colliders, const AABB* blockingPlayer)
{
    const float step = GameConfig::CRATE_MAX_STEP_HEIGHT;
    const AABB headroom = makeAABB(
        box.center + glm::vec2{0.0f, box.halfSize.y + step * 0.5f}, {m_size.x, step});
    if (blockingPlayer && intersects(headroom, *blockingPlayer)) {
        return groundDx;
    }
    for (const SceneCollider& contact : colliders) {
        if (contact.pushable() != this && intersects(headroom, contact.bounds())) {
            return groundDx;
        }
    }

    AABB raised = box;
    raised.center.y += step;
    const float raisedDx = allowedHorizontalMove(raised, roomLimitedDx, colliders, blockingPlayer);
    if (std::abs(raisedDx) <= std::abs(groundDx)) {
        return groundDx;
    }

    m_position.y = raised.center.y;
    m_onGround = false;
    return raisedDx;
}

void Crate::moveWithSupport(float deltaY)
{
    m_position.y += deltaY;
}

void Crate::reset()
{
    m_position = m_spawnCenter;
    m_velocityY = 0.0f;
    m_onGround = false;
}
