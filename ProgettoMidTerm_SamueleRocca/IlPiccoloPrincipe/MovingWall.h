#pragma once
#include "LevelTypes.h"
#include "LinearTransition.h"

class MovingWall {
public:
    /**
     * @param closedCenter Centro della parete all'inizio in coordinate mondo.
     * @param openCenter Centro della parete alla fine in coordinate mondo.
     * @param size Larghezza e altezza.
     * @param travelSeconds Durata di una corsa completa in secondi.
     *
     * Crea la parete in posizione iniziale.
     */
    MovingWall(const glm::vec2& closedCenter, const glm::vec2& openCenter,
               const glm::vec2& size, float travelSeconds);

    /**
     * @return Centro attuale della parete.
     *
     * Restituisce il centro attuale, interpolato tra le posizioni iniziali e finali.
     */
    glm::vec2 position() const;

    /**
     * @return Solido con centro e dimensioni correnti.
     *
     * Restituisce il solido nella posizione attuale, con larghezza e altezza complete.
     */
    SolidCollider collider() const;

    /**
     * @param dt Intervallo di simulazione in secondi.
	 * @param activated true se la parete deve aprirsi, false se deve chiudersi.
     *
     * Avanza di dt secondi verso la posizione finale se il comando è attivo, altrimenti verso
     * quella iniziale. Con travelSeconds zero o negativa, il movimento è immediato. Un dt zero o negativo
     * lascia la parete nella posizione attuale.
     */
    void update(float dt, bool activated);

    /**
     * Riporta la parete e il suo collider alla posizione iniziale.
     */
    void reset();

    /**
     * @return True se la parete si trova tra le posizioni estreme, false altrimenti.
     *
     * Controlla se il progresso della parete è compreso strettamente tra zero e uno.
     */
    bool isBetweenStops() const { return m_transition.progress() > 0.0f && m_transition.progress() < 1.0f; }

private:
    glm::vec2 m_closedCenter;
    glm::vec2 m_openCenter;
    glm::vec2 m_size;
    float m_travelSeconds;
    LinearTransition m_transition;
};
