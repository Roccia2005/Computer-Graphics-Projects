#include "Level.h"
#include "PlayerPhysics.h"
#include "SceneCollider.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

using namespace GameConfig;

namespace {

    /**
     * @param wall Collider iniziale della parete.
     * @param nextPosition Centro della parete al termine del passo.
     * @param body Rettangolo di collisione del corpo.
     * @return true se il percorso della parete interseca il corpo.
     *
     * Verifica se il rettangolo della parete attraversa un corpo durante il movimento. Il semplice
     * contatto sui bordi non blocca la corsa.
     */
    bool wallPathIntersects(const SolidCollider& wall, const glm::vec2& nextPosition,
                            const AABB& body)
    {
        const glm::vec2 delta = nextPosition - wall.position;
        if (delta == glm::vec2(0.0f)) {
            return false;
        }

        const glm::vec2 halfSize = body.halfSize + wall.size * 0.5f;
        const glm::vec2 offset = wall.position - body.center;
        float entryFraction = 0.0f;
        float exitFraction = 1.0f;

        for (glm::length_t axis = 0; axis < 2; ++axis) {
            if (delta[axis] == 0.0f) {
                if (std::abs(offset[axis]) >= halfSize[axis]) {
                    return false;
                }
                continue;
            }

            const float first = (-halfSize[axis] - offset[axis]) / delta[axis];
            const float last = (halfSize[axis] - offset[axis]) / delta[axis];
            entryFraction = std::max(entryFraction, std::min(first, last));
            exitFraction = std::min(exitFraction, std::max(first, last));
            if (entryFraction >= exitFraction) {
                return false;
            }
        }
        return true;
    }
}

Level::Level(const LevelDefinition& definition)
    : m_roomHalfSize(definition.roomHalfSize), m_floorY(definition.floorY),
      m_exitDoor(definition.doorBase), m_requireAllStarsToExit(definition.requireAllStarsToExit),
      m_timeLimitSeconds(definition.timeLimitSeconds), m_remainingSeconds(m_timeLimitSeconds)
{
    for (std::size_t i = 0; i < m_players.size(); ++i) {
        m_players[i].spawnAtFeet(definition.spawnFeet[i]);
    }

    for (const auto& platform : definition.platforms) {
        const glm::vec2 center = platform.basePosition + glm::vec2(0.0f, platform.size.y * 0.5f);
        m_platforms.push_back({center, platform.size});
    }
    for (const auto& position : definition.stars) {
        m_stars.push_back({position});
    }
    for (const auto& plate : definition.pressurePlates) {
        m_pressurePlates.emplace_back(plate);
    }

    for (const auto& wall : definition.movingWalls) {
        const bool invalidLink = std::any_of(
            wall.pressurePlateIndices.begin(), wall.pressurePlateIndices.end(),
            [&](std::size_t index) { return index >= m_pressurePlates.size(); });
        if (invalidLink) {
            throw std::out_of_range("Indice pedana non valido per la parete mobile");
        }

        m_movingWalls.push_back({
            MovingWall(wall.closedCenter, wall.openCenter, wall.size, wall.travelSeconds),
            wall.pressurePlateIndices
        });
    }
    for (const EnemyDefinition& enemy : definition.enemies) {
        m_enemies.emplace_back(enemy);
    }
    for (const CrateDefinition& crate : definition.crates) {
        m_crates.emplace_back(crate);
    }
}

/**
 * @return Elenco dei solidi da usare nel passo di fisica.
 *
 * Riunisce piattaforme, pedane, pareti mobili e casse per risolvere le collisioni.
 */
std::vector<SceneCollider> Level::sceneColliders()
{
    std::vector<SceneCollider> colliders;
    colliders.reserve(m_platforms.size() + m_pressurePlates.size() + m_movingWalls.size()
                  + m_crates.size() + m_players.size());
    for (const SolidCollider& platform : m_platforms) {
        colliders.emplace_back(platform);
    }
    for (const PressurePlate& plate : m_pressurePlates) {
        colliders.emplace_back(plate.collider());
    }
    for (const LinkedMovingWall& link : m_movingWalls) {
        colliders.emplace_back(link.wall.collider());
    }
    for (Crate& crate : m_crates) {
        colliders.emplace_back(crate);
    }
    return colliders;
}

/**
 * @param dt Intervallo di simulazione in secondi.
 * @param colliders Solidi coinvolti nel passo di simulazione.
 * @param step Passo di fisica dei giocatori.
 *
 * Fa cadere le casse, aggiorna i collider e trasmette il movimento ai giocatori appoggiati.
 */
void Level::updateCrates(float dt, std::vector<SceneCollider>& colliders, PlayerPhysics::Step& step)
{
    const auto roomColliderCount = colliders.size();
    for (Crate& crate : m_crates) {
        for (const Player& player : m_players) {
            const AABB body = player.bounds();
            colliders.emplace_back(SolidCollider{body.center, body.halfSize * 2.0f});
        }
        const float previousCenterY = crate.bounds().center.y;
        crate.fall(dt, m_floorY, colliders);
        colliders.erase(colliders.begin() + roomColliderCount, colliders.end());
        step.followCrate(crate, {0.0f, crate.bounds().center.y - previousCenterY});
    }
}

void Level::update(float dt, const PlayerInput& input1, const PlayerInput& input2,
                   float currentTime)
{
    auto colliders = sceneColliders();
    PlayerPhysics::Step step(m_players, colliders, m_roomHalfSize.x, m_floorY);
    if (dt > 0.0f) {
        const std::array<PlayerInput, 2> inputs{input1, input2};
        step.prepare(inputs);
        updateCrates(dt, colliders, step);
        step.applyPosture(inputs);
        step.integrate(dt);
    }
    updatePressurePlates(colliders, step);
    const bool playerHit = updateMovingWalls(dt) || updateEnemies(dt);
    if (playerHit) {
        reset();
        return;
    }
    updateStarsAndExit(dt, currentTime);
    if (m_remainingSeconds) {
        *m_remainingSeconds = std::max(0.0f, *m_remainingSeconds - dt);
        if (*m_remainingSeconds == 0.0f && !isFinished()) {
            reset();
        }
    }
}

/**
 * @param player Giocatore interessato dalla verifica o dal movimento.
 * @return true se il giocatore interseca l'area di uscita.
 *
 * Restituisce true quando il rettangolo del giocatore si sovrappone all'area di uscita.
 */
bool Level::isPlayerAtExit(const Player& player) const
{
    return m_exitDoor.overlaps(player.bounds());
}

/**
 * @param colliders Solidi coinvolti nel passo di simulazione.
 * @param step Passo di fisica dei giocatori.
 *
 * Per ogni pedana rileva giocatori e casse appoggiati e controlla lo spazio per la risalita.
 * Applica agli oggetti appoggiati lo spostamento verticale restituito dalla pedana.
 */
void Level::updatePressurePlates(std::vector<SceneCollider>& colliders, PlayerPhysics::Step& step)
{
    for (std::size_t i = 0; i < m_pressurePlates.size(); ++i) {
        PressurePlate& plate = m_pressurePlates[i];
        const SolidCollider solid = plate.collider();
        const AABB plateBox = makeAABB(solid.position, solid.size);

        const bool playerOnPlate = step.capturePlateSupports(plateBox);
        std::vector<Crate*> supportedCrates;
        for (Crate& crate : m_crates) {
            if (isRestingOn(crate.bounds(), plateBox, 0.0f)) supportedCrates.push_back(&crate);
        }
        const bool plateIsLoaded = playerOnPlate || !supportedCrates.empty();
        bool releaseBlocked = false;
        if (plate.isPressed() && !plateIsLoaded) {
            const SolidCollider raised = plate.releasedCollider();
            const AABB raisedBox = makeAABB(raised.position, raised.size);

            releaseBlocked = std::any_of(
                m_players.begin(), m_players.end(), [&](const Player& player) {
                    return intersects(player.bounds(), raisedBox);
                }) || std::any_of(m_crates.begin(), m_crates.end(), [&](const Crate& crate) {
                    return intersects(crate.bounds(), raisedBox);
                });
        }

        const float deltaY = plate.update(plateIsLoaded, releaseBlocked);
        if (deltaY == 0.0f) {
            continue;
        }

        colliders[m_platforms.size() + i] = SceneCollider(plate.collider());
        for (Crate* crate : supportedCrates) {
            crate->moveWithSupport(deltaY);
        }
        for (const Crate* crate : supportedCrates) {
            step.followCrate(*crate, {0.0f, deltaY});
        }
        step.followPlate(deltaY);
    }
}

/**
 * @param dt Intervallo di simulazione in secondi.
 * @return true se una parete in chiusura schiaccia un giocatore.
 *
 * Legge lo stato appena aggiornato delle pedane. Basta una pedana premuta per aprire la parete.
 * La parete si ferma contro una cassa. Se colpisce un giocatore in chiusura, segnala il reset.
 */
bool Level::updateMovingWalls(float dt)
{
    for (LinkedMovingWall& link : m_movingWalls) {
        link.moving = false;
        const bool activated = std::any_of(
            link.pressurePlateIndices.begin(), link.pressurePlateIndices.end(),
            [&](std::size_t index) { return m_pressurePlates[index].isPressed(); });

        MovingWall proposedWall = link.wall;
        proposedWall.update(dt, activated);

        const SolidCollider current = link.wall.collider();
        const glm::vec2 nextPosition = proposedWall.position();
        const bool playerInPath = std::any_of(
            m_players.begin(), m_players.end(), [&](const Player& player) {
                return wallPathIntersects(current, nextPosition, player.bounds());
            });
        if (playerInPath) {
            if (!activated) return true;
            continue;
        }
        const bool crateInPath = std::any_of(m_crates.begin(), m_crates.end(), [&](const Crate& crate) {
            return wallPathIntersects(current, nextPosition, crate.bounds());
        });
        if (crateInPath) continue;

        link.wall = proposedWall;
        link.moving = current.position != nextPosition && link.wall.isBetweenStops();
    }
    return false;
}

/**
 * @param dt Intervallo di simulazione in secondi.
 * @return true se un nemico tocca un giocatore.
 *
 * Fa avanzare la pattuglia e segnala il contatto con uno dei due giocatori.
 */
bool Level::updateEnemies(float dt)
{
    for (Enemy& enemy : m_enemies) {
        enemy.update(dt);
        const AABB enemyBounds = enemy.bounds();
        const bool touchesPlayer = std::any_of(
            m_players.begin(), m_players.end(), [&](const Player& player) {
                return intersects(player.bounds(), enemyBounds);
            });
        if (touchesPlayer) {
            return true;
        }
    }
    return false;
}

/**
 * @param currentTime Tempo di gioco in secondi.
 * @return true se almeno una stella è stata raccolta in questo passo.
 *
 * Entrambi i giocatori possono raccogliere le stelle alla loro posizione oscillante.
 */
bool Level::collectStars(float currentTime)
{
    bool collectedAnyStar = false;
    for (std::size_t i = 0; i < m_stars.size(); ++i) {
        Collectible& star = m_stars[i];
        if (star.collected) continue;
        glm::vec2 animatedPosition = star.position;
        const float phase = starPhase(i);
        animatedPosition.y += STAR_BOB_AMPLITUDE *
            std::sin(STAR_BOB_FREQUENCY * currentTime + phase);

        for (const Player& player : m_players) {
            const AABB body = player.bounds();
            const glm::vec2 closestPoint = glm::clamp(animatedPosition,
                body.center - body.halfSize, body.center + body.halfSize);
            const glm::vec2 toStar = animatedPosition - closestPoint;
            if (glm::dot(toStar, toStar) <= STAR_RADIUS * STAR_RADIUS) {
                star.collected = true;
                collectedAnyStar = true;
                break;
            }
        }
    }
    return collectedAnyStar;
}

/**
 * @param dt Intervallo di simulazione in secondi.
 * @param currentTime Tempo di gioco usato per la raccolta e per l'avviso di sblocco.
 *
 * Registra lo sblocco quando viene raccolta l'ultima stella richiesta e aggiorna
 * l'apertura della porta in base alla presenza di un giocatore all'uscita.
 */
void Level::updateStarsAndExit(float dt, float currentTime)
{
    const bool collectedAnyStar = collectStars(currentTime);
    const bool exitUnlocked = exitEnabled(collectedStarCount());
    if (collectedAnyStar && m_requireAllStarsToExit && exitUnlocked) {
        m_exitUnlockedAt = currentTime;
    }
    m_exitDoor.update(dt, exitUnlocked && exitOccupied());
}

/**
 * @return true se almeno un giocatore occupa l'uscita.
 *
 * Controlla se almeno un giocatore si trova nell'area della porta.
 */
bool Level::exitOccupied() const
{
    return isPlayerAtExit(m_players[0]) || isPlayerAtExit(m_players[1]);
}

/**
 * @return Numero di stelle raccolte.
 *
 * Conta le stelle contrassegnate come raccolte nella stanza.
 */
std::size_t Level::collectedStarCount() const
{
    return std::count_if(m_stars.begin(), m_stars.end(),
                         [](const Collectible& star) { return star.collected; });
}

/**
 * @param collected Numero di stelle già raccolte.
 * @return true se la raccolta consente di usare la porta.
 *
 * Verifica il requisito delle stelle previsto dalla stanza.
 */
bool Level::exitEnabled(std::size_t collected) const
{
    return !m_requireAllStarsToExit || collected == m_stars.size();
}

StarProgress Level::starProgress() const
{
    const auto collected = collectedStarCount();
    return {collected, m_stars.size(), !exitEnabled(collected) && exitOccupied(), m_exitUnlockedAt};
}

bool Level::isFinished() const
{
    return m_exitDoor.isOpen() && exitEnabled(collectedStarCount())
        && isPlayerAtExit(m_players[0]) && isPlayerAtExit(m_players[1]);
}

void Level::reset()
{
    m_remainingSeconds = m_timeLimitSeconds;
    m_exitDoor.reset();
    m_exitUnlockedAt.reset();
    for (Player& player : m_players) {
        player.reset();
    }
    for (PressurePlate& plate : m_pressurePlates) {
        plate.reset();
    }
    for (LinkedMovingWall& link : m_movingWalls) {
        link.wall.reset();
        link.moving = false;
    }
    for (Enemy& enemy : m_enemies) {
        enemy.reset();
    }
    for (Crate& crate : m_crates) {
        crate.reset();
    }
    for (Collectible& star : m_stars) {
        star.collected = false;
    }
}
