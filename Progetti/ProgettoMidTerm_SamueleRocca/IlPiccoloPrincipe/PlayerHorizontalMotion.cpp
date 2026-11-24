#include "PlayerPhysics.h"
#include "SceneCollider.h"
#include "Pushable.h"
#include <algorithm>
#include <cmath>

using namespace GameConfig;

namespace PlayerPhysics {

/**
 * @param playerIndex Indice del giocatore nella coppia.
 * @param requested Spostamento orizzontale richiesto prima delle collisioni.
 * @return Spostamento con segno consentito dai confini e dai solidi fissi.
 *
 * Calcola la corsa possibile contro i solidi fissi prima di risolvere contatti e spinte.
 */
float Step::distanceAgainstFixedSolids(std::size_t playerIndex, float requested) const
{
    const AABB body = m_players[playerIndex].bounds();
    const float limit = m_roomHalfWidth - body.halfSize.x;
    float distance = std::clamp(body.center.x + requested, -limit, limit)
                   - body.center.x;
    for (const SceneCollider& contact : m_colliders) {
        if (!contact.pushable()) {
            distance = horizontalSweepLimit(body, distance, contact.bounds(), PenetrationResponse::BlockForwardMotion);
        }
    }
    return distance;
}

/**
 * @param i Indice del giocatore nella coppia.
 * @param dt Intervallo di simulazione in secondi.
 * @return Spostamento orizzontale totale richiesto.
 *
 * Somma alla corsa del giocatore il trasporto orizzontale ancora da applicare.
 */
float Step::requestedDistance(std::size_t i, float dt) const
{
    return m_players[i].velocity().x * dt + m_states[i].pendingCarryX;
}

/**
 * @param i Indice del giocatore nella coppia.
 * @param distance Spostamento orizzontale richiesto, con segno.
 * @param requested Corsa originaria prima delle limitazioni dovute ai contatti.
 *
 * Risolve urti e spinte, poi applica la corsa effettiva e la trasmette al passeggero.
 */
void Step::moveHorizontally(std::size_t i, float distance, float requested)
{
    Player& player = m_players[i];
    const Player& other = m_players[1 - i];
    const AABB body = player.bounds();
    distance = horizontalSweepLimit(body, distance, other.bounds(), PenetrationResponse::BlockForwardMotion);
    for (const SceneCollider& contact : m_colliders) {
        Pushable* obstacle = contact.pushable();
        if (!obstacle) continue;
        const AABB before = contact.bounds();
        const float gap = horizontalSweepLimit(body, distance, before, PenetrationResponse::BlockForwardMotion);
        if (std::abs(gap) < std::abs(distance)) {
            const AABB otherBody = other.bounds();
            obstacle->push(distance - gap, m_roomHalfWidth, m_colliders,
                           m_states[1 - i].support.crate == obstacle ? nullptr : &otherBody);
            const AABB after = contact.bounds();
            followCrate(*obstacle, after.center - before.center);
            distance = horizontalSweepLimit(body, distance, after, PenetrationResponse::BlockForwardMotion);
        }
    }
    player.translate({distance, 0.0f});
    if (m_states[1 - i].support.kind == SupportKind::OtherPlayer) {
        m_states[1 - i].pendingCarryX += player.position().x - body.center.x;
    }
    if (std::abs(distance - requested) > contactTolerance) player.stopHorizontalMotion();
}

/**
 * @param playerIndex Indice del giocatore che potrebbe trasportare il compagno.
 * @param dt Intervallo di simulazione in secondi.
 * @return true se il compagno poggia sul giocatore oppure se il giocatore raggiunge
 *         con la spinta la cassa su cui poggia il compagno.
 *
 * Se entrambi poggiano sulla stessa cassa, il supporto condiviso non dà precedenza a nessuno.
 */
bool Step::movesOtherPlayerSupport(std::size_t playerIndex, float dt) const
{
    const CarrySupport& otherSupport = m_states[1 - playerIndex].support;
    if (otherSupport.kind == SupportKind::OtherPlayer) {
        return true;
    }
    if (!otherSupport.crate || otherSupport.crate == m_states[playerIndex].support.crate) {
        return false;
    }
    const auto crate = otherSupport.crate->collider();
    const float distance = distanceAgainstFixedSolids(playerIndex, requestedDistance(playerIndex, dt));
    return horizontalSweepLimit(m_players[playerIndex].bounds(), distance,
        makeAABB(crate.position, crate.size), PenetrationResponse::BlockForwardMotion) != distance;
}

/**
 * @param playerIndex Indice del giocatore da muovere.
 * @param dt Intervallo di simulazione in secondi.
 *
 * Calcola la corsa con il trasporto accumulato fino a questo momento e risolve urti e spinte.
 */
void Step::movePlayerHorizontally(std::size_t playerIndex, float dt)
{
    const float requested = requestedDistance(playerIndex, dt);
    const float distance = distanceAgainstFixedSolids(playerIndex, requested);
    moveHorizontally(playerIndex, distance, requested);
}

/**
 * @param dt Intervallo di simulazione in secondi.
 *
 * Determina l'ordine di movimento dei giocatori e limita la corsa quando si incontrano.
 */
void Step::movePlayersHorizontally(float dt)
{
    for (std::size_t playerIndex = 0; playerIndex < m_players.size(); ++playerIndex) {
        if (movesOtherPlayerSupport(playerIndex, dt)) {
            const std::size_t otherIndex = 1 - playerIndex;
            movePlayerHorizontally(playerIndex, dt);
            movePlayerHorizontally(otherIndex, dt);
            return;
        }
    }

    const std::size_t left = m_players[0].position().x <= m_players[1].position().x ? 0 : 1;
    const std::size_t right = 1 - left;
    const AABB leftBody = m_players[left].bounds(), rightBody = m_players[right].bounds();
    const float leftRequested = requestedDistance(left, dt), rightRequested = requestedDistance(right, dt);
    float leftDistance = distanceAgainstFixedSolids(left, leftRequested);
    float rightDistance = distanceAgainstFixedSolids(right, rightRequested);
    if (leftDistance > 0.0f && rightDistance < 0.0f &&
        overlapsVertically(leftBody, rightBody, contactTolerance)) {
        const float gap = std::max(0.0f, rightBody.center.x - leftBody.center.x
                                 - leftBody.halfSize.x - rightBody.halfSize.x);
        const float fraction = std::min(1.0f, gap / (leftDistance - rightDistance));
        leftDistance *= fraction;
        rightDistance *= fraction;
    }
    if (leftDistance > 0.0f && rightDistance > 0.0f) {
        moveHorizontally(right, rightDistance, rightRequested);
        moveHorizontally(left, leftDistance, leftRequested);
    } else {
        moveHorizontally(left, leftDistance, leftRequested);
        moveHorizontally(right, rightDistance, rightRequested);
    }
}

}
