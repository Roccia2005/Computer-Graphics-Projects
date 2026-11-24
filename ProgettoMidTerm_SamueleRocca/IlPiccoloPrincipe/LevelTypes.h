#pragma once
#include "Config.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <optional>

struct AABB { glm::vec2 center; glm::vec2 halfSize; };
inline constexpr float contactTolerance = 0.001f;

/**
 * @param a Primo rettangolo di collisione.
 * @param b Secondo rettangolo di collisione.
 * @param tolerance Sovrapposizione minima richiesta lungo l'asse.
 * @return true se la sovrapposizione orizzontale supera la tolleranza.
 *
 * Confronta gli intervalli occupati dai due rettangoli sull'asse X.
 */
inline bool overlapsHorizontally(const AABB& a, const AABB& b, float tolerance)
{
    return std::abs(a.center.x - b.center.x) < a.halfSize.x + b.halfSize.x - tolerance;
}

/**
 * @param a Primo rettangolo di collisione.
 * @param b Secondo rettangolo di collisione.
 * @param tolerance Sovrapposizione minima richiesta lungo l'asse.
 * @return true se la sovrapposizione verticale supera la tolleranza.
 *
 * Confronta gli intervalli occupati dai due rettangoli sull'asse Y.
 */
inline bool overlapsVertically(const AABB& a, const AABB& b, float tolerance)
{
    return std::abs(a.center.y - b.center.y) < a.halfSize.y + b.halfSize.y - tolerance;
}

/**
 * @param body Rettangolo di collisione del corpo.
 * @param support Superficie o corpo di appoggio.
 * @param horizontalTolerance Sovrapposizione orizzontale minima richiesta per l'appoggio.
 * @return true se il corpo poggia sul supporto entro le tolleranze.
 *
 * Confronta la quota dei piedi con la sommità del supporto e verifica l'appoggio orizzontale.
 */
inline bool isRestingOn(const AABB& body, const AABB& support, float horizontalTolerance)
{
    const float feet = body.center.y - body.halfSize.y;
    const float top = support.center.y + support.halfSize.y;
    return overlapsHorizontally(body, support, horizontalTolerance) &&
           std::abs(feet - top) <= contactTolerance;
}

enum class PenetrationResponse { BlockForwardMotion, IgnoreBeyondTolerance };

/**
 * @param body Rettangolo di collisione del corpo.
 * @param distance Spostamento orizzontale richiesto, con segno.
 * @param obstacle Rettangolo del solido che limita il movimento.
 * @param response Regola da applicare alle compenetrazioni iniziali.
 * @return Spostamento orizzontale consentito, con segno.
 *
 * Limita il movimento verso l'ostacolo alla distanza che precede il contatto.
 */
inline float horizontalSweepLimit(const AABB& body, float distance, const AABB& obstacle,
                                  PenetrationResponse response)
{
    if (!overlapsVertically(body, obstacle, contactTolerance)) {
        return distance;
    }
    const float direction = distance > 0.0f ? 1.0f : -1.0f;
    const float centerGap = (obstacle.center.x - body.center.x) * direction;
    const float gap = centerGap - body.halfSize.x - obstacle.halfSize.x;
    if (response == PenetrationResponse::BlockForwardMotion ? centerGap <= 0.0f
                                                            : gap < -contactTolerance) {
        return distance;
    }
    return direction * std::min(std::abs(distance), std::max(0.0f, gap));
}

/**
 * @param a Primo rettangolo di collisione.
 * @param b Secondo rettangolo di collisione.
 * @return true se i rettangoli si sovrappongono su entrambi gli assi.
 *
 * Il solo contatto sul bordo lascia nulla l'area di intersezione.
 */
inline bool intersects(const AABB& a, const AABB& b)
{
    return overlapsHorizontally(a, b, 0.0f) && overlapsVertically(a, b, 0.0f);
}
/**
 * @param center Centro nel sistema di coordinate di destinazione.
 * @param size Larghezza e altezza.
 * @return Rettangolo espresso tramite centro e semidimensioni.
 *
 * Costruisce il rettangolo di collisione dimezzando le dimensioni complete.
 */
inline AABB makeAABB(const glm::vec2& center, const glm::vec2& size)
{
    return { center, size * 0.5f };
}
struct SolidCollider {
    glm::vec2 position{0.0f};
    glm::vec2 size{0.0f};
};
struct Collectible {
    glm::vec2 position{0.0f};
    bool collected = false;
};
struct StarProgress {
    std::size_t collected;
    std::size_t total;
    bool atLockedExit;
    std::optional<float> unlockedAt;
};

struct PlayerInput {
    bool left = false;
    bool right = false;
    bool jumpHeld = false;
    bool down = false;
};
