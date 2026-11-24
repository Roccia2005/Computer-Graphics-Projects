#pragma once
#include "LevelTypes.h"

struct EnemyDefinition;

class Enemy {
public:
    /**
     * @param definition Configurazione iniziale definita nella mappa.
     *
     * Inizializza il nemico al punto di partenza della pattuglia.
     */
    explicit Enemy(const EnemyDefinition& definition);

    /**
     * @param dt Intervallo di simulazione in secondi.
     *
     * Avanza di dt secondi lungo il percorso e inverte il moto agli estremi.
     */
    void update(float dt);

    /**
     * @return Rettangolo di collisione corrente.
     *
     * Restituisce centro e semidimensioni del nemico in coordinate mondo.
     */
    AABB bounds() const;

    /**
     * Riporta il nemico alla posizione iniziale e ripristina la direzione di partenza.
     */
    void reset();

private:
    glm::vec2 m_startCenter;
    glm::vec2 m_endCenter;
    glm::vec2 m_size;
    float m_speed;
    double m_pathPhase = 0.0;
};
