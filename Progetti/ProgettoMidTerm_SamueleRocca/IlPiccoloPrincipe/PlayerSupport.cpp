#include "PlayerPhysics.h"
#include "SceneCollider.h"
#include "Pushable.h"
#include <algorithm>

using namespace GameConfig;

namespace {
/**
 * @param player Giocatore interessato dalla verifica o dal movimento.
 * @param support Superficie o corpo di appoggio.
 * @return true se il giocatore è appoggiato al supporto indicato.
 *
 * Controlla il contatto dei piedi con la superficie per un giocatore a terra.
 */
bool isSupportedBy(const Player& player, const AABB& support)
{
    return player.isOnGround() && isRestingOn(player.bounds(), support, contactTolerance);
}
}

namespace PlayerPhysics {

/**
 * Identifica il compagno o la cassa che sostiene ciascun giocatore.
 */
void Step::captureSupports()
{
    for (std::size_t i = 0; i < m_players.size(); ++i) {
        PlayerStepState& state = m_states[i];
        state.support = {};
        const Player& player = m_players[i];
        if (isSupportedBy(player, m_players[1 - i].bounds())) {
            state.support = {SupportKind::OtherPlayer};
            continue;
        }
        for (const SceneCollider& solid : m_colliders) {
            if (solid.pushable() && isSupportedBy(player, solid.bounds())) {
                state.support = {SupportKind::Crate, solid.pushable()};
                break;
            }
        }
    }
}

bool Step::capturePlateSupports(const AABB& plate)
{
    captureSupports();
    bool pressed = false;
    for (std::size_t i = 0; i < m_players.size(); ++i) {
        if (isSupportedBy(m_players[i], plate)) {
            pressed = true;
            if (m_states[i].support.kind == SupportKind::None) m_states[i].support = {SupportKind::Plate};
        }
    }
    return pressed;
}

/**
 * @param i Indice del giocatore nella coppia.
 * @param delta Spostamento verticale da applicare, con segno.
 *
 * Sposta in verticale il giocatore e il rettangolo di riferimento del passo.
 */
void Step::translateY(std::size_t i, float delta)
{
    m_players[i].translate({0.0f, delta});
    m_states[i].verticalReferenceBounds.center.y += delta;
}

/**
 * @param i Indice del giocatore nella coppia.
 * @param support Superficie o corpo di appoggio.
 *
 * Porta i piedi del giocatore alla quota superiore del supporto.
 */
void Step::alignWithSupport(std::size_t i, const AABB& support)
{
    const float top = support.center.y + support.halfSize.y;
    const AABB body = m_players[i].bounds();
    const float feet = body.center.y - body.halfSize.y;
    translateY(i, top - feet);
}

/**
 * @param i Indice del giocatore nella coppia.
 * @param delta Spostamento verticale richiesto dal supporto.
 * @return Spostamento verticale realmente applicato.
 *
 * Trasporta il giocatore in verticale e interrompe la discesa se incontra un altro appoggio.
 */
float Step::followVertically(std::size_t i, float delta)
{
    if (delta < 0.0f) {
        const AABB body = m_players[i].bounds();
        const float feet = body.center.y - body.halfSize.y;
        float landing = m_floorY;
        const auto consider = [&](const AABB& solid) {
            const float top = solid.center.y + solid.halfSize.y;
            if (top <= feet + contactTolerance && overlapsHorizontally(body, solid, contactTolerance)) {
                landing = std::max(landing, top);
            }
        };
        for (const SceneCollider& solid : m_colliders) {
            if (solid.pushable() && solid.pushable() == m_states[i].support.crate) continue;
            consider(solid.bounds());
        }
        if (m_states[i].support.kind != SupportKind::OtherPlayer) consider(m_players[1 - i].bounds());
        if (feet + delta < landing) {
            delta = landing - feet;
            m_states[i].support = {};
        }
    }
    translateY(i, delta);
    return delta;
}

/**
 * @param support Superficie o corpo di appoggio.
 * @param delta Spostamento effettivo del supporto, in unità mondo.
 *
 * Trasmette il moto del supporto ai passeggeri e tiene conto di un giocatore sulle spalle.
 */
void Step::follow(CarrySupport support, glm::vec2 delta)
{
    if (delta == glm::vec2(0.0f)) return;
    for (std::size_t i = 0; i < m_players.size(); ++i) {
        PlayerStepState& state = m_states[i];
        if (state.support.kind != support.kind || state.support.crate != support.crate) continue;
        state.pendingCarryX += delta.x;
        const float movedY = followVertically(i, delta.y);
        if (m_states[1 - i].support.kind == SupportKind::OtherPlayer) {
            followVertically(1 - i, movedY);
        }
    }
}

void Step::followCrate(const Pushable& crate, glm::vec2 delta)
{
    follow({SupportKind::Crate, &crate}, delta);
}

void Step::followPlate(float deltaY)
{
    follow({SupportKind::Plate}, {0.0f, deltaY});
}

}
