#pragma once
#include "LevelTypes.h"
#include "Player.h"
#include <array>
#include <vector>

class Pushable;
class SceneCollider;

namespace PlayerPhysics {

/**
 * Aggiorna la fisica dei due giocatori durante un passo di simulazione.
 * Level crea uno Step a ogni aggiornamento. Lo Step modifica i giocatori del livello
 * e tiene traccia dei loro appoggi e degli spostamenti ricevuti da casse, pedane o compagno.
 */
class Step {
public:
    /**
     * @param players Coppia di giocatori da aggiornare.
     * @param colliders Solidi coinvolti nel passo di simulazione.
     * @param roomHalfWidth Metà della larghezza della stanza in unità mondo.
     * @param floorY Quota del pavimento in unità mondo.
     *
     * Collega i giocatori e i solidi della stanza per un passo della simulazione.
     */
    Step(std::array<Player, 2>& players, const std::vector<SceneCollider>& colliders,
         float roomHalfWidth, float floorY);
    /**
     * @param inputs Comandi dei due giocatori, nello stesso ordine dei personaggi.
     *
     * Memorizza gli ingombri iniziali, applica i comandi e individua gli appoggi.
     */
    void prepare(const std::array<PlayerInput, 2>& inputs);
    /**
     * @param inputs Comandi dei due giocatori, nello stesso ordine dei personaggi.
     *
     * Verifica lo spazio disponibile per abbassare o rialzare i due giocatori.
     */
    void applyPosture(const std::array<PlayerInput, 2>& inputs);
    /**
     * @param dt Intervallo di simulazione in secondi.
     *
     * Risolve il movimento orizzontale e poi quello verticale dei giocatori.
     */
    void integrate(float dt);

    /**
     * @param crate Cassa di cui seguire lo spostamento.
     * @param delta Spostamento effettivo del supporto, in unità mondo.
     *
     * Trasmette ai passeggeri lo spostamento effettivo della cassa.
     */
    void followCrate(const Pushable& crate, glm::vec2 delta);
    /**
     * @param plate Rettangolo della superficie della pedana.
     * @return true se almeno un giocatore poggia sulla pedana.
     *
     * Individua chi poggia sulla pedana e registra la pedana come supporto per il trasporto.
     */
    bool capturePlateSupports(const AABB& plate);
    /**
     * @param deltaY Variazione della quota del supporto in unità mondo.
     *
     * Sposta in verticale i giocatori sostenuti dalla pedana.
     */
    void followPlate(float deltaY);

private:
    enum class SupportKind { None, OtherPlayer, Crate, Plate };

    struct CarrySupport {
        SupportKind kind = SupportKind::None;
        const Pushable* crate = nullptr;
    };

    struct PlayerStepState {
        CarrySupport support{};
        AABB verticalReferenceBounds{};
        float pendingCarryX = 0.0f;
    };

    std::array<Player, 2>& m_players;
    const std::vector<SceneCollider>& m_colliders;
    float m_roomHalfWidth;
    float m_floorY;
    std::array<PlayerStepState, 2> m_states{};

    void movePlayersHorizontally(float dt);
    bool movesOtherPlayerSupport(std::size_t playerIndex, float dt) const;
    void movePlayerHorizontally(std::size_t playerIndex, float dt);
    float requestedDistance(std::size_t i, float dt) const;
    float distanceAgainstFixedSolids(std::size_t playerIndex, float requested) const;
    void moveHorizontally(std::size_t i, float distance, float requested);

    void movePlayersVertically(float dt);

    void captureSupports();
    void alignWithSupport(std::size_t i, const AABB& support);
    void translateY(std::size_t i, float delta);
    float followVertically(std::size_t i, float delta);
    void follow(CarrySupport support, glm::vec2 delta);
};

}
