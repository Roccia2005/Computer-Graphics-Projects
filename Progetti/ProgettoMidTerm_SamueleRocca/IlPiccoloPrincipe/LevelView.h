#pragma once
#include "Scene.h"
#include <array>
#include <optional>
#include <vector>

class ExitDoor;
class Level;
class PressurePlate;
struct AABB;
struct SolidCollider;

class LevelView {
public:
    /**
     * Inizializza la vista caricando le geometrie condivise fra le stanze. Richiede un contesto
     * OpenGL attivo.
     */
    LevelView();
    /**
     * @param level Stanza da rappresentare.
     * @param levelNumber Numero della stanza, con zero riservato al tutorial.
     * @param timeSeconds Tempo corrente dell'animazione o della simulazione, in secondi.
     *
     * Sincronizza le istanze con lo stato corrente della stanza. Crea le istanze al primo disegno e
     * le ricostruisce quando cambia il numero della stanza.
     */
    void sync(const Level& level, std::size_t levelNumber, float timeSeconds);
    /**
     * @return Riferimento in sola lettura alla scena.
     *
     * Espone le geometrie e le istanze predisposte per il disegno.
     */
    const Scene& scene() const { return m_scene; }

private:
    struct PressurePlateInstances {
        std::size_t pressed;
        std::size_t released;
    };
    struct LevelGeometries {
        std::size_t solidRectangle;
        std::size_t movingWall;
        std::size_t star;
        std::size_t snake;
        std::size_t crate;
        std::array<std::size_t, 2> players;
        std::size_t doorRecess;
        std::array<std::size_t, 2> doorLeaves;
        std::size_t doorFrame;
        std::size_t plateBase;
        std::size_t plateReleased;
        std::size_t platePressed;
    };

    Scene m_scene;
    LevelGeometries m_geometries;
    std::optional<std::size_t> m_loadedLevelNumber;
    std::array<std::size_t, 2> m_playerInstances{};
    std::array<std::size_t, 2> m_doorLeafInstances{};
    std::vector<PressurePlateInstances> m_pressurePlateInstances;
    std::vector<std::size_t> m_movingWallInstances;
    std::vector<std::size_t> m_enemyInstances;
    std::vector<std::size_t> m_crateInstances;
    std::vector<std::size_t> m_starInstances;

    LevelGeometries loadGeometries();
    void rebuildInstances(const Level& level);
    void createDoor(const AABB& exitArea);
    void createPlatform(const SolidCollider& platform);
    void createPressurePlate(const PressurePlate& plate);
    void updatePlayers(const Level& level);
    void updateStars(const Level& level, float timeSeconds);
    void updateDoorLeaves(const ExitDoor& door);
};
