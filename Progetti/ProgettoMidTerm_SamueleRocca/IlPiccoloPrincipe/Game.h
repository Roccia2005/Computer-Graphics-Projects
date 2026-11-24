#pragma once
#include "Level.h"
#include <cstddef>

class Game {
public:
    /**
     * Crea la partita inizializzando lo stato della prima stanza dalle definizioni dei livelli.
     */
    Game();
    /**
     * @param dt Intervallo di simulazione in secondi.
     * @param input1 Comandi del primo giocatore.
     * @param input2 Comandi del secondo giocatore.
     * @param currentTime Tempo di gioco in secondi.
     *
     * Fa avanzare la stanza di dt secondi con le azioni dei due giocatori. Se raggiungono l'uscita,
     * carica la stanza successiva oppure segna la partita come conclusa.
     */
    void update(float dt, const PlayerInput& input1, const PlayerInput& input2,
                float currentTime);
    /**
     * Ripristina lo stato iniziale della stanza corrente e riattiva la partita anche dopo il
     * completamento.
     */
    void restartCurrentLevel();
    /**
     * @return Livello attivo in sola lettura.
     *
     * Espone la stanza attiva della campagna.
     */
    const Level& level() const { return m_level; }
    /**
     * @return Numero della stanza corrente.
     *
     * Converte l'indice interno nel numero di livello mostrato al giocatore.
     */
    std::size_t levelNumber() const { return m_levelIndex + 1; }
    /**
     * @return true dopo il completamento dell'ultimo livello.
     *
     * Indica se la campagna è stata completata.
     */
    bool finished() const { return m_finished; }

private:
    Level m_level;
    std::size_t m_levelIndex = 0;
    bool m_finished = false;
};
