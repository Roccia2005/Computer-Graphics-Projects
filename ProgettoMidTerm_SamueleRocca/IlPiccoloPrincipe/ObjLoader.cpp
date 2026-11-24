#include "ObjLoader.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <charconv>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace {
using Palette = std::unordered_map<std::string, glm::vec3>;

/**
 * @param path Percorso del file dei materiali MTL.
 * @param palette Tabella dei colori da aggiornare per nome del materiale.
 *
 * Legge i colori diffusi dal file MTL e li associa ai nomi dei materiali.
 */
void loadMaterials(const std::filesystem::path& path, Palette& palette)
{
    std::ifstream file(path);
    if (!file) throw std::runtime_error("Materiale non trovato: " + path.string());
    std::string line, name;
    while (std::getline(file, line)) {
        std::istringstream row(line);
        std::string tag;
        row >> tag;
        if (tag == "newmtl") {
            row >> name;
        } else if (tag == "Kd" && !name.empty()) {
            glm::vec3 rgb;
            if (!(row >> rgb.r >> rgb.g >> rgb.b))
                throw std::runtime_error("Colore Kd non valido in " + path.string());
            palette[name] = rgb;
        }
    }
}

/**
 * @param token Riferimento a un vertice nella definizione di una faccia OBJ.
 * @param count Numero di posizioni già lette dal file OBJ.
 * @return Indice a base zero del vertice.
 *
 * Risolve l'indice di posizione OBJ, anche relativo o seguito da /uv/normale.
 */
std::size_t positionIndex(const std::string& token, std::size_t count)
{
    const char* begin = token.data();
    const char* end = begin + std::min(token.size(), token.find('/'));
    std::ptrdiff_t index = 0;
    const auto parsed = std::from_chars(begin, end, index);
    if (parsed.ec != std::errc{} || parsed.ptr != end || index == 0)
        throw std::runtime_error("Indice OBJ non valido: " + token);
    index = index > 0 ? index - 1 : static_cast<std::ptrdiff_t>(count) + index;
    if (index < 0 || static_cast<std::size_t>(index) >= count)
        throw std::runtime_error("Indice OBJ fuori intervallo: " + token);
    return static_cast<std::size_t>(index);
}
}

MeshData ObjLoader::load(const std::filesystem::path& path)
{
    std::ifstream file(path);
    if (!file) throw std::runtime_error("Modello non trovato: " + path.string());
    MeshData mesh;
    std::vector<glm::vec2> positions;
    Palette palette;
    glm::vec3 color(1.0f);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream row(line);
        std::string tag;
        row >> tag;
        if (tag == "v") {
            glm::vec2 point;
            if (!(row >> point.x >> point.y))
                throw std::runtime_error("Vertice non valido in " + path.string());
            positions.push_back(point);
        } else if (tag == "mtllib") {
            std::string name;
            while (row >> name && name.front() != '#')
                loadMaterials(path.parent_path() / name, palette);
        } else if (tag == "usemtl") {
            std::string name;
            row >> name;
            const auto material = palette.find(name);
            if (material == palette.end())
                throw std::runtime_error("Materiale " + name + " sconosciuto in " + path.string());
            color = material->second;
        } else if (tag == "f") {
            std::vector<std::size_t> face;
            std::string token;
            while (row >> token && token.front() != '#')
                face.push_back(positionIndex(token, positions.size()));
            for (std::size_t i = 2; i < face.size(); ++i) {
                const auto& a = positions[face.front()];
                const auto& b = positions[face[i - 1]];
                const auto& c = positions[face[i]];
                const float cross = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
                if (std::abs(cross) >= 1e-10f)
                    mesh.addTriangle(a, b, c, color);
            }
        }
    }
    if (mesh.vertices.empty()) throw std::runtime_error("Modello senza triangoli: " + path.string());

    return mesh;
}
