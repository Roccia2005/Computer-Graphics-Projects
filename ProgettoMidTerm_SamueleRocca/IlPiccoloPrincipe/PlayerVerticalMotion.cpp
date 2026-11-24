#include "PlayerPhysics.h"
#include "SceneCollider.h"
#include "Pushable.h"
#include <algorithm>

using namespace GameConfig;

namespace {
/**
 * @param player Giocatore interessato dalla verifica o dal movimento.
 * @param colliders Solidi coinvolti nel passo di simulazione.
 *
 * Corregge le compenetrazioni verticali con i solidi. Azzera la velocità verticale e segna
 * l'appoggio quando il giocatore atterra sulla superficie.
 */
void verticalCollision(Player& player, const std::vector<SceneCollider>& colliders)
{
    const float verticalVelocity = player.velocity().y;
    for (const SceneCollider& contact : colliders) {
        const AABB playerBox = player.bounds();
        const AABB solidBox = contact.bounds();
        if (!overlapsHorizontally(playerBox, solidBox, contactTolerance) || !intersects(playerBox, solidBox)) {
            continue;
        }
        if (verticalVelocity <= 0.0f) {
            player.landOn(solidBox.center.y + solidBox.halfSize.y);
        }
        else {
            player.hitCeilingAt(solidBox.center.y - solidBox.halfSize.y);
        }
    }
}

struct VerticalBounds {
    float feet;
    float head;
};

/**
 * @param bounds Centro e semidimensioni del rettangolo.
 * @return Quote inferiore e superiore.
 *
 * Ricava le quote dei piedi e della testa del giocatore.
 */
VerticalBounds verticalBounds(const AABB& bounds)
{
    return {bounds.center.y - bounds.halfSize.y, bounds.center.y + bounds.halfSize.y};
}

/**
 * @param player Giocatore interessato dalla verifica o dal movimento.
 * @return Quote inferiore e superiore.
 *
 * Ricava le quote dei piedi e della testa del giocatore.
 */
VerticalBounds verticalBounds(const Player& player)
{
    return verticalBounds(player.bounds());
}

/**
 * @param upper Giocatore superiore.
 * @param lower Giocatore inferiore.
 * @param upperBefore Quote del giocatore superiore prima del movimento.
 * @param lowerBefore Quote del giocatore inferiore prima del movimento.
 *
 * Usa le quote prima e dopo il movimento per risolvere l'appoggio sulla testa e gli urti dal
 * basso. Se rialzarsi causerebbe una sovrapposizione, ripristina l'altezza precedente.
 */
void resolvePlayerVerticalContact(Player& upper, Player& lower,
                                  VerticalBounds upperBefore, VerticalBounds lowerBefore)
{
    constexpr float verticalContactTolerance = 0.0001f;
    const float gapBefore = upperBefore.feet - lowerBefore.head;
    if (gapBefore < -verticalContactTolerance ||
        !overlapsHorizontally(upper.bounds(), lower.bounds(), contactTolerance)) {
        return;
    }

    const float upperFeet = verticalBounds(upper).feet;
    auto lowerNow = verticalBounds(lower);
    if (upperFeet > lowerNow.head) {
        return;
    }

    const float previousHeight = lowerBefore.head - lowerBefore.feet;
    if (lower.height() > previousHeight + verticalContactTolerance) {
        lower.setHeight(previousHeight);
        lowerNow = verticalBounds(lower);
    }

    if (upperFeet > lowerNow.head) {
        return;
    }

    if (!lower.isOnGround() && lowerNow.head > lowerBefore.head + verticalContactTolerance) {
        float contactY = upperFeet;
        if (!upper.isOnGround() && upperFeet < upperBefore.feet) {
            const float distanceBefore = std::max(gapBefore, 0.0f);
            const float penetration = lowerNow.head - upperFeet;
            const float contactFraction = distanceBefore > 0.0f ? distanceBefore / (distanceBefore + penetration) : 0.0f;
            contactY = lowerBefore.head + (lowerNow.head - lowerBefore.head) * contactFraction;
            upper.landOn(contactY);
        }
        lower.hitCeilingAt(contactY);
    } else if (upper.velocity().y <= 0.0f && upperFeet <= upperBefore.feet) {
        upper.landOn(lowerNow.head);
    }
}

/**
 * @param player Giocatore interessato dalla verifica o dal movimento.
 * @param other Secondo giocatore da considerare per le collisioni.
 * @param input Comandi correnti del giocatore.
 * @param colliders Solidi coinvolti nel passo di simulazione.
 *
 * Applica la postura richiesta quando lo spazio sopra la testa permette il rialzo. Controlla
 * solo la parte aggiunta alla sagoma, così l'appoggio ai piedi resta valido.
 */
void updatePlayerPosture(Player& player, const Player& other, const PlayerInput& input,
                         const std::vector<SceneCollider>& colliders)
{
    const float height = player.requestedHeight(input);
    if (height == player.height()) {
        return;
    }

    if (height > player.height()) {
        const AABB current = player.bounds();
        const AABB taller = player.boundsAtHeight(height);
        const float head = current.center.y + current.halfSize.y;
        const float tallerHead = taller.center.y + taller.halfSize.y;
        const AABB headroom = makeAABB({current.center.x, (head + tallerHead) * 0.5f},
                                      {current.halfSize.x * 2.0f, tallerHead - head});
        const AABB otherBody = other.bounds();
        if (overlapsHorizontally(headroom, otherBody, contactTolerance) && intersects(headroom, otherBody)) {
            return;
        }
        const bool blocked = std::any_of(colliders.begin(), colliders.end(),
            [&](const SceneCollider& contact) {
                return intersects(headroom, contact.bounds());
            });
        if (blocked) {
            return;
        }
    }

    player.setHeight(height);
}

/**
 * @param player Giocatore interessato dalla verifica o dal movimento.
 * @param dt Intervallo di simulazione in secondi.
 * @param floorY Quota del pavimento in unità mondo.
 * @param colliders Solidi coinvolti nel passo di simulazione.
 *
 * Applica gravità e collisioni verticali dopo aver risolto il movimento orizzontale.
 */
void moveVertically(Player& player, float dt, float floorY, const std::vector<SceneCollider>& colliders)
{
    const float halfPlayerHeight = player.bounds().halfSize.y;

    player.advanceVertically(dt);

    if (player.position().y - halfPlayerHeight <= floorY) {
        player.landOn(floorY);
    }

    verticalCollision(player, colliders);
}
}

namespace PlayerPhysics {

void Step::applyPosture(const std::array<PlayerInput, 2>& inputs)
{
    for (std::size_t i = 0; i < m_players.size(); ++i) {
        updatePlayerPosture(m_players[i], m_players[1 - i], inputs[i], m_colliders);
    }
    captureSupports();
    for (std::size_t i = 0; i < m_players.size(); ++i) {
        if (m_states[i].support.crate) {
            const auto solid = m_states[i].support.crate->collider();
            alignWithSupport(i, makeAABB(solid.position, solid.size));
        }
    }
    for (std::size_t i = 0; i < m_players.size(); ++i) {
        if (m_states[i].support.kind == SupportKind::OtherPlayer) alignWithSupport(i, m_players[1 - i].bounds());
    }
}

/**
 * @param dt Intervallo di simulazione in secondi.
 *
 * Muove entrambi i giocatori in verticale e risolve il contatto tra i due.
 */
void Step::movePlayersVertically(float dt)
{
    for (std::size_t i = 0; i < m_players.size(); ++i) {
        Player& player = m_players[i];
        if (m_states[i].support.crate) {
            const SolidCollider solid = m_states[i].support.crate->collider();
            const AABB support = makeAABB(solid.position, solid.size);
            if (isRestingOn(player.bounds(), support, contactTolerance)) {
                alignWithSupport(i, support);
                player.stopVerticalMotion();
                continue;
            }
        }
        moveVertically(player, dt, m_floorY, m_colliders);
    }
    for (std::size_t i = 0; i < m_players.size(); ++i) {
        resolvePlayerVerticalContact(m_players[i], m_players[1 - i],
            verticalBounds(m_states[i].verticalReferenceBounds), verticalBounds(m_states[1 - i].verticalReferenceBounds));
    }
}

}
