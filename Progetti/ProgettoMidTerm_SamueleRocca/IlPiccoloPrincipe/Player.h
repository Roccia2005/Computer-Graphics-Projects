#pragma once
#include "LevelTypes.h"

class Player {
public:
    /**
     * @return Altezza del giocatore nella postura attuale, in unità mondo.
     */
    float height() const { return m_height; }
    /**
     * @return Centro del giocatore in coordinate mondo.
     */
    glm::vec2 position() const { return m_position; }
    /**
     * @return Velocità orizzontale e verticale in unità mondo al secondo.
     */
    glm::vec2 velocity() const { return m_velocity; }
    /**
     * @return true se il giocatore poggia su una superficie o sul compagno.
     */
    bool isOnGround() const { return m_onGround; }

    /**
     * @param feet Posizione dei piedi indicata dalla mappa.
     *
     * Ricava il centro di spawn dalla posizione dei piedi e lo memorizza per i successivi reset.
     * Colloca subito il giocatore in quel punto, in piedi e con velocità nulla.
     */
    void spawnAtFeet(glm::vec2 feet);
    /**
     * Riporta il giocatore allo spawn, in piedi e con velocità nulla.
     */
    void reset();
    /**
     * @param input Comandi correnti del giocatore.
     *
     * Imposta la velocità orizzontale dai tasti premuti e avvia il salto se c'è un appoggio. Il
     * salto libera l'appoggio prima che venga valutata la postura richiesta.
     */
    void applyInput(const PlayerInput& input);
    /**
     * @param input Comandi correnti del giocatore.
     * @return Altezza desiderata nella postura richiesta.
     *
     * Il comando di abbassamento vale a terra, durante il salto e in caduta.
     */
    float requestedHeight(const PlayerInput& input) const;
    /**
     * @return Rettangolo di collisione nella postura attuale.
     */
    AABB bounds() const;
    /**
     * @param height Altezza in unità mondo.
     * @return Rettangolo di collisione corrispondente all'altezza proposta.
     *
     * Calcola il rettangolo di una nuova postura mantenendo la quota dei piedi e lo stato attuale.
     */
    AABB boundsAtHeight(float height) const;
    /**
     * @param height Altezza in unità mondo.
     *
     * Cambia l'altezza e sposta il centro quanto basta per tenere fermi i piedi.
     */
    void setHeight(float height);
    /**
     * @param dt Intervallo di simulazione in secondi.
     *
     * Aggiorna la velocità verticale con la gravità e la usa per calcolare la nuova quota.
     * Il giocatore torna in appoggio se la verifica delle collisioni rileva un atterraggio.
     */
    void advanceVertically(float dt);
    /**
     * @param surfaceY Quota della superficie su cui atterrano i piedi.
     *
     * Porta i piedi alla quota della superficie, azzera la velocità verticale e segna
     * il giocatore come appoggiato.
     */
    void landOn(float surfaceY);
    /**
     * @param surfaceY Quota della superficie urtata con la testa.
     *
     * Allinea la testa alla superficie e ferma il moto verticale.
     */
    void hitCeilingAt(float surfaceY);
    /**
     * @param delta Spostamento da applicare sui due assi, in unità mondo.
     *
     * Sposta il centro del giocatore mantenendo la velocità e lo stato di appoggio.
     */
    void translate(glm::vec2 delta);
    /**
     * Azzera la velocità orizzontale del giocatore.
     */
    void stopHorizontalMotion();
    /**
     * Azzera la velocità verticale del giocatore.
     */
    void stopVerticalMotion();

private:
    float m_height = GameConfig::PLAYER_HEIGHT;
    glm::vec2 m_spawnPosition{0.0f};
    glm::vec2 m_position{0.0f};
    glm::vec2 m_velocity{0.0f};
    bool m_onGround = true;
};
