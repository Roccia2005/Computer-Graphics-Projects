#pragma once
#include "LevelTypes.h"

struct PressurePlateDefinition;

class PressurePlate {
public:
    /**
     * @param definition Configurazione iniziale definita nella mappa.
     *
     * Crea una pedana rilasciata con la posizione, le dimensioni e la corsa definite nella mappa.
     */
    explicit PressurePlate(const PressurePlateDefinition& definition);

    /**
     * @return Centro del bordo inferiore.
     *
     * Restituisce il centro della base della pedana in coordinate mondo.
     */
    const glm::vec2& basePosition() const { return m_basePosition; }
    /**
     * @return Dimensioni complete a riposo.
     *
     * Restituisce la larghezza e l'altezza della pedana a riposo.
     */
    const glm::vec2& size() const { return m_size; }
    /**
     * @return di quanto scende la superficie quando la pedana viene premuta.
     */
    float pressDistance() const { return m_pressDistance; }
    /**
     * @return True se la pedana è premuta, false altrimenti.
     */
    bool isPressed() const { return m_pressed; }

    /**
     * @return Solido di collisione con centro e dimensioni correnti.
     */
    SolidCollider collider() const;
    /**
     * @return Solido di collisione che la pedana occupa a riposo.
     */
    SolidCollider releasedCollider() const;
    /**
     * @param hasSupport True se un giocatore o una cassa poggia sulla pedana, false altrimenti.
     * @param releaseBlocked True se un ostacolo impedisce alla superficie di risalire, false altrimenti.
     * @return Spostamento verticale della superficie, negativo in discesa.
     *
     * Abbassa la pedana sotto un carico e la rialza quando il passaggio è libero.
     */
    float update(bool hasSupport, bool releaseBlocked);
    /**
     * Ripristina lo stato rilasciato e il relativo collider durante il reset del livello.
     */
    void reset();

private:
    glm::vec2 m_basePosition;
    glm::vec2 m_size;
    float m_pressDistance;
    bool m_pressed = false;

    float surfaceY(bool pressed) const;
    SolidCollider colliderFor(bool pressed) const;
};
