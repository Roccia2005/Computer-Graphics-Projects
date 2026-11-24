#pragma once
#include "LevelTypes.h"

class Pushable;

class SceneCollider {
public:
    /**
     * @param solid Solido fisso da inserire nella scena delle collisioni.
     *
     * Costruisce un riferimento di collisione per la simulazione.
     */
    explicit SceneCollider(SolidCollider solid) : m_fixed(solid) {}
    /**
     * @param body Corpo spingibile di cui seguire la posizione.
     *
     * Costruisce un riferimento di collisione per la simulazione.
     */
    explicit SceneCollider(Pushable& body) : m_pushable(&body) {}

    /**
     * @return Solido con centro e dimensioni correnti.
     *
     * Legge il solido memorizzato o quello aggiornato del corpo spingibile.
     */
    SolidCollider collider() const;
    /**
     * @return Rettangolo di collisione corrente.
     *
     * Calcola il rettangolo di collisione a partire dal solido corrente.
     */
    AABB bounds() const
    {
        const SolidCollider solid = collider();
        return makeAABB(solid.position, solid.size);
    }
    /**
     * @return Puntatore al corpo spingibile, oppure nullptr per un solido fisso.
     *
     * Permette di applicare una spinta quando il corpo la supporta.
     */
    Pushable* pushable() const { return m_pushable; }

private:
    SolidCollider m_fixed{};
    Pushable* m_pushable = nullptr;
};
