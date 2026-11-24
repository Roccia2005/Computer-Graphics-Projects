#pragma once
#include "LevelTypes.h"
#include "LinearTransition.h"

class ExitDoor {
public:
    /**
     * @param basePosition Centro del bordo inferiore in coordinate mondo.
     *
     * Crea una porta chiusa. Costruisce l'area di uscita dal centro del bordo inferiore e dalle
     * dimensioni della porta in GameConfig.
     */
    explicit ExitDoor(const glm::vec2& basePosition);

    /**
     * @return Rettangolo dell'uscita.
     *
     * Restituisce l'area di uscita in coordinate mondo.
     */
    const AABB& area() const { return m_area; }
    /**
     * @return Frazione di apertura tra zero e uno.
     *
     * Restituisce la frazione di apertura, da 0 (chiusa) a 1 (completamente aperta).
     */
    float opening() const { return m_transition.progress(); }
    /**
     * @return true ad apertura completa.
     *
     * Restituisce true quando la porta ha raggiunto l'apertura completa.
     */
    bool isOpen() const { return m_transition.progress() >= 1.0f; }
    /**
     * @param body Rettangolo di collisione del corpo.
     * @return true se i rettangoli condividono un'area positiva.
     *
     * Verifica se il rettangolo di body si sovrappone all'area di uscita. I bordi a contatto
     * delimitano rettangoli senza area in comune.
     */
    bool overlaps(const AABB& body) const;

    /**
     * @param dt Intervallo di simulazione in secondi.
     * @param shouldOpen true per aprire la porta, false per richiuderla.
     *
     * Fa avanzare l'apertura di dt secondi su richiesta del livello, altrimenti fa avanzare la
     * chiusura. Una transizione completa dura GameConfig::DOOR_OPENING_SECONDS.
     */
    void update(float dt, bool shouldOpen);
    /**
     * Riporta subito la porta allo stato chiuso.
     */
    void reset();

private:
    AABB m_area;
    LinearTransition m_transition;
};
