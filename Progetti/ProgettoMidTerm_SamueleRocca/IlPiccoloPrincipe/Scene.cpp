#include "Scene.h"
#include <stdexcept>

std::size_t Scene::addGeometry(const MeshData& mesh)
{
    m_geometries.push_back(std::make_unique<Geometry>(mesh));
    return m_geometries.size() - 1;
}

std::size_t Scene::addInstance(std::size_t geometryIndex, MaterialId material)
{
    if (geometryIndex >= m_geometries.size()) {
        throw std::out_of_range("geometryIndex non valido");
    }
    GeometryInstance instance;
    instance.geometryIndex = geometryIndex;
    instance.material = material;
    m_instances.push_back(instance);
    return m_instances.size() - 1;
}

GeometryInstance& Scene::instance(std::size_t index)
{
    return m_instances.at(index);
}

const Geometry& Scene::geometry(std::size_t index) const
{
    return *m_geometries.at(index);
}

const std::vector<GeometryInstance>& Scene::instances() const
{
    return m_instances;
}

void Scene::clearInstances()
{
    m_instances.clear();
}

