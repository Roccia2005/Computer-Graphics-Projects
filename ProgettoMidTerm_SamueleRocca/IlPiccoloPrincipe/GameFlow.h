#pragma once
#include "DialogueSpeaker.h"
#include "Game.h"
#include <cstddef>
#include <optional>

enum class Screen {
    MainMenu,
    IntroDialogue,
    Story,
    Tutorial,
    FinalDialogue
};

enum class UiAction {
    None,
    StartStory,
    StartTutorial,
    AdvanceDialogue,
    BackToMenu,
    RestartRoom
};

class GameFlow {
public:
    /**
     * @param action Azione richiesta dall'interfaccia o dalla tastiera.
     * @param timeSeconds Tempo corrente della partita in secondi.
     *
     * Applica l'azione consentita nella schermata corrente e gestisce i cambi di schermata.
     */
    void applyAction(UiAction action, float timeSeconds);
    /**
     * @param dt Intervallo di simulazione in secondi.
     * @param timeSeconds Tempo corrente della partita in secondi.
     * @param first Comandi del primo giocatore.
     * @param second Comandi del secondo giocatore.
     *
     * Aggiorna la campagna o il tutorial e avvia il dialogo finale al completamento.
     */
    void update(float dt, float timeSeconds, const PlayerInput& first,
                const PlayerInput& second);

    /**
     * @return Schermata attiva.
     *
     * Indica quale schermata deve essere mostrata.
     */
    Screen screen() const { return m_screen; }

    /**
     * @return Puntatore alla battuta corrente, oppure nullptr.
     *
     * Restituisce la battuta da mostrare, se c'è un dialogo in corso.
     */
    const DialogueLine* dialogueLine() const;
    /**
     * @return Puntatore alla stanza disponibile, oppure nullptr.
     *
     * Espone la stanza da disegnare, compresa l'ultima stanza durante l'epilogo.
     */
    const Level* levelToDraw() const;

    /**
     * @return Numero della stanza corrente.
     *
     * Numero della stanza restituita da levelToDraw(). Il tutorial vale 0.
     */
    std::size_t levelNumber() const;
    /**
     * @return Tempo della simulazione in secondi.
     *
     * Tempo usato dalla simulazione e dall'animazione delle stelle della stanza.
     */
    float worldTime() const { return m_worldTime; }

private:
    Screen m_screen = Screen::MainMenu;
    std::optional<Game> m_story;
    std::optional<Level> m_tutorial;
    std::size_t m_dialogueIndex = 0;
    float m_worldTime = 0.0f;
};
