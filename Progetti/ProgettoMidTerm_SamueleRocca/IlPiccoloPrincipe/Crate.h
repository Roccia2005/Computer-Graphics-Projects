#pragma once
#include "LevelTypes.h"
#include "Pushable.h"

struct CrateDefinition;

class Crate : public Pushable {
public:
    /**
     * @param definition Configurazione iniziale definita nella mappa.
     *
     * Posiziona la cassa alla quota di base definita dalla mappa.
     */
    explicit Crate(const CrateDefinition& definition);

    /**
     * @return Solido con centro e dimensioni correnti.
     *
     * Restituisce il solido corrente, con centro e dimensioni complete.
     */
    SolidCollider collider() const override;
    /**
     * @return Rettangolo di collisione corrente.
     *
     * Restituisce il rettangolo di collisione usato per pedane e appoggi.
     */
    AABB bounds() const;

    /**
     * @param dt Intervallo di simulazione in secondi.
     * @param floorY Quota del pavimento in unità mondo.
     * @param scene Solidi coinvolti nel passo di simulazione.
     *
     * Applica la gravità e ferma la discesa sulla prima superficie attraversata.
     */
    void fall(float dt, float floorY, const std::vector<SceneCollider>& scene);
    /**
     * @param distance Spostamento orizzontale richiesto, con segno.
     * @param roomHalfWidth Metà della larghezza della stanza in unità mondo.
     * @param colliders Solidi coinvolti nel passo di simulazione.
     * @param blockingPlayer Ingombro del compagno che può bloccare la spinta, oppure nullptr.
     *
     * Applica la spinta orizzontale consentita da solidi, confini e altro giocatore.
     */
    void push(float distance, float roomHalfWidth, const std::vector<SceneCollider>& colliders,
              const AABB* blockingPlayer) override;
    /**
     * @param deltaY Variazione della quota del supporto in unità mondo.
     *
     * Segue lo spostamento verticale della superficie di una pedana.
     */
    void moveWithSupport(float deltaY);
    /**
     * Ripristina posizione e velocità iniziali.
     */
    void reset();

private:
    float allowedHorizontalMove(const AABB& body, float roomLimitedDx,
                                const std::vector<SceneCollider>& colliders, const AABB* blockingPlayer) const;
    float tryStepUp(const AABB& box, float roomLimitedDx, float groundDx,
                    const std::vector<SceneCollider>& colliders, const AABB* blockingPlayer);

    glm::vec2 m_spawnCenter;
    glm::vec2 m_position;
    glm::vec2 m_size;
    float m_velocityY = 0.0f;
    bool m_onGround = false;
};
