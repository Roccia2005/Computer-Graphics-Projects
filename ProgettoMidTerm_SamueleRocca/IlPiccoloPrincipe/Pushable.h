#pragma once
#include <vector>

struct AABB;
struct SolidCollider;
class SceneCollider;

class Pushable {
public:
    /**
     * Permette di distruggere il corpo attraverso l'interfaccia comune.
     */
    virtual ~Pushable() = default;
    /**
     * @return Solido di collisione con centro e dimensioni correnti.
     */
    virtual SolidCollider collider() const = 0;
    /**
     * @param distance Spostamento orizzontale richiesto, con segno.
     * @param roomHalfWidth Metà della larghezza della stanza in unità mondo.
     * @param colliders Solidi di collisione coinvolti nel passo di simulazione.
     * @param blockingPlayer Ingombro del compagno che può bloccare la spinta, oppure nullptr.
     *
     * Richiede uno spostamento orizzontale limitato dagli ostacoli e dai confini.
     */
    virtual void push(float distance, float roomHalfWidth, const std::vector<SceneCollider>& colliders,
                      const AABB* blockingPlayer) = 0;
};
