#pragma once
#include "Geometry.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <cstddef>
enum class MaterialId { Basic, WallIdle, WallMoving, Star };

enum class SceneLayer { World, Foreground };

struct GeometryInstance {
    SceneLayer layer = SceneLayer::World;
    std::size_t geometryIndex=0;
    glm::mat4 modelMatrix{1.0f};
    bool visible=true;
    MaterialId material = MaterialId::Basic;
};
class Scene {
    std::vector<std::unique_ptr<Geometry>> m_geometries;
    std::vector<GeometryInstance> m_instances;
public:
    /**
     * @param mesh Vertici da caricare nella memoria della GPU.
     * @return Indice della nuova geometria.
     *
     * Crea una geometria GPU posseduta dalla scena e restituisce l'indice da usare per creare
     * istanze.
     */
    std::size_t addGeometry(const MeshData& mesh);
    /**
     * @param geometryIndex Indice della geometria condivisa nella scena.
     * @param material Materiale con cui disegnare l'istanza.
     * @return Indice della nuova istanza.
     *
     * Aggiunge un'istanza visibile con matrice identita' e il materiale richiesto. Restituisce il
     * suo indice e segnala un indice mesh non valido con std::out_of_range.
     */
    std::size_t addInstance(std::size_t geometryIndex, MaterialId material = MaterialId::Basic);
    /**
     * @param index Indice dell'elemento, a partire da zero.
     * @return Istanza modificabile all'indice richiesto.
     *
     * Restituisce un riferimento modificabile all'istanza. Un indice fuori intervallo genera
     * std::out_of_range.
     */
    GeometryInstance& instance(std::size_t index);
    /**
     * @param index Indice dell'elemento, a partire da zero.
     * @return Geometria condivisa in sola lettura.
     *
     * Restituisce la geometria condivisa in sola lettura. Un indice fuori intervallo genera
     * std::out_of_range.
     */
    const Geometry& geometry(std::size_t index) const;
    /**
     * @return Istanze in ordine di inserimento.
     *
     * Espone le istanze in sola lettura, nell'ordine di inserimento usato dai renderer.
     */
    const std::vector<GeometryInstance>& instances() const;
    /**
     * Rimuove le istanze. Gli indici delle geometrie restano validi.
     */
    void clearInstances();
};
