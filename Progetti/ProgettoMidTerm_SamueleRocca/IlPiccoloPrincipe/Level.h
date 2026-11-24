#pragma once
#include "LevelDefinition.h"
#include "LevelTypes.h"
#include "Crate.h"
#include "ExitDoor.h"
#include "Enemy.h"
#include "MovingWall.h"
#include "Player.h"
#include "PressurePlate.h"
#include <optional>

class SceneCollider;
namespace PlayerPhysics { class Step; }

class Level {
public:
    /**
     * @param definition Configurazione iniziale definita nella mappa.
     *
     * Costruisce lo stato iniziale della stanza a partire dalla mappa. Imposta confini, spawn, area
     * di uscita, piattaforme, stelle, pedane, pareti mobili, nemici, casse e tempo disponibile.
     */
    explicit Level(const LevelDefinition& definition);
    /**
     * @param dt Intervallo di simulazione in secondi.
     * @param input1 Comandi del primo giocatore.
     * @param input2 Comandi del secondo giocatore.
     * @param currentTime Istante di gioco in secondi per la raccolta e l'avviso di sblocco.
     *
     * Risolve movimento e pericoli. Un contatto con un nemico o uno schiacciamento riavvia
     * subito la stanza. Dopo la raccolta e l'apertura della porta sottrae dt al tempo
     * disponibile. Alla scadenza riavvia la stanza solo se i giocatori devono ancora completarla.
     */
    void update(float dt, const PlayerInput& input1, const PlayerInput& input2,
                float currentTime);
    /**
     * Ripristina giocatori, porta, pedane, pareti, nemici, casse, stelle e tempo iniziale.
     */
    void reset();
    /**
     * @return Secondi residui del tentativo, oppure std::nullopt nelle stanze senza limite.
     */
    std::optional<float> remainingSeconds() const { return m_remainingSeconds; }
    /**
     * @return true se entrambe le figure hanno raggiunto la porta aperta e abilitata.
     *
     * Restituisce true quando la porta è abilitata, completamente aperta ed entrambi i giocatori
     * sono nell'area di uscita.
     */
    bool isFinished() const;
    /**
     * @return Conteggio delle stelle, stato di blocco e istante dello sblocco.
     *
     * Raccoglie i dati necessari al contatore delle stelle e agli avvisi sulla porta.
     */
    StarProgress starProgress() const;

    /**
     * @return Metà della larghezza e dell'altezza in unità mondo.
     *
     * Fornisce le dimensioni usate per inquadrare la stanza.
     */
    const glm::vec2& roomHalfSize() const { return m_roomHalfSize; }
    /**
     * @return Ordinata del pavimento.
     *
     * Indica a quale altezza si trova il pavimento.
     */
    float floorY() const { return m_floorY; }
    /**
     * @return I due giocatori in sola lettura.
     *
     * Permette di leggere lo stato dei due personaggi.
     */
    const std::array<Player, 2>& players() const { return m_players; }
    /**
     * @return Elenco dei collider delle piattaforme.
     *
     * Espone i solidi fissi disposti nella stanza.
     */
    const std::vector<SolidCollider>& platforms() const { return m_platforms; }
    /**
     * @return Elenco delle stelle della stanza.
     *
     * Permette di leggere posizione e stato di raccolta di ogni stella.
     */
    const std::vector<Collectible>& stars() const { return m_stars; }
    /**
     * @return Casse nella posizione corrente.
     *
     * Espone le casse per disegnarle nelle loro posizioni correnti.
     */
    const std::vector<Crate>& crates() const { return m_crates; }
    /**
     * @return Nemici della stanza.
     *
     * Espone i nemici per disegnarli nelle loro posizioni correnti.
     */
    const std::vector<Enemy>& enemies() const { return m_enemies; }
    /**
     * @return Pedane nell'ordine della mappa.
     *
     * Espone le pedane della stanza in sola lettura, nell'ordine definito nella mappa.
     */
    const std::vector<PressurePlate>& pressurePlates() const { return m_pressurePlates; }
    /**
     * @return Numero di pareti mobili.
     *
     * Restituisce il numero di pareti mobili della stanza.
     */
    std::size_t movingWallCount() const { return m_movingWalls.size(); }
    /**
     * @param index Indice dell'elemento, a partire da zero.
     * @return Parete corrispondente all'indice.
     *
     * Espone una parete in sola lettura, tramite il suo indice nella mappa.
     */
    const MovingWall& movingWall(std::size_t index) const { return m_movingWalls.at(index).wall; }
    /**
     * @param index Indice dell'elemento, a partire da zero.
     * @return true se la parete è in movimento.
     *
     * Controlla se la parete si è spostata nell'ultimo passo ed è tra i due estremi.
     */
    bool isMovingWallInMotion(std::size_t index) const { return m_movingWalls.at(index).moving; }
    /**
     * @return Porta di uscita della stanza.
     *
     * Espone la porta di uscita in sola lettura, compresa la frazione di apertura.
     */
    const ExitDoor& exitDoor() const { return m_exitDoor; }

private:
    struct LinkedMovingWall {
        MovingWall wall;
        std::vector<std::size_t> pressurePlateIndices;
        bool moving = false;
    };

    glm::vec2 m_roomHalfSize;
    float m_floorY;
    std::array<Player, 2> m_players{};
    std::vector<SolidCollider> m_platforms;
    std::vector<Collectible> m_stars;
    std::vector<PressurePlate> m_pressurePlates;
    std::vector<LinkedMovingWall> m_movingWalls;
    std::vector<Enemy> m_enemies;
    std::vector<Crate> m_crates;
    ExitDoor m_exitDoor;
    bool m_requireAllStarsToExit;
    std::optional<float> m_exitUnlockedAt;
    std::optional<float> m_timeLimitSeconds;
    std::optional<float> m_remainingSeconds;

    bool isPlayerAtExit(const Player& player) const;
    bool exitOccupied() const;
    std::size_t collectedStarCount() const;
    bool exitEnabled(std::size_t collected) const;
    std::vector<SceneCollider> sceneColliders();
    void updateCrates(float dt, std::vector<SceneCollider>& colliders, PlayerPhysics::Step& step);
    void updatePressurePlates(std::vector<SceneCollider>& colliders, PlayerPhysics::Step& step);
    bool updateMovingWalls(float dt);
    bool updateEnemies(float dt);
    bool collectStars(float currentTime);
    void updateStarsAndExit(float dt, float currentTime);
};
