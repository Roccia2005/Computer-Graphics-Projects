#include "DecorativeShapes.h"
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <utility>

namespace {

/**
 * @param mesh Mesh alla quale accodare i due triangoli.
 * @param a Primo vertice.
 * @param b Secondo vertice.
 * @param c Terzo vertice.
 * @param d Quarto vertice.
 * @param color Colore RGB dei vertici.
 *
 * Divide in due triangoli un quadrilatero convesso con vertici ordinati lungo il bordo.
 */
void addQuad(MeshData& mesh, const glm::vec2& a, const glm::vec2& b,
             const glm::vec2& c, const glm::vec2& d, const glm::vec3& color)
{
    mesh.addTriangle(a, b, c, color);
    mesh.addTriangle(a, c, d, color);
}

/**
 * @param mesh Mesh alla quale aggiungere il rettangolo.
 * @param xMin Ascissa del bordo sinistro.
 * @param yMin Ordinata del bordo inferiore.
 * @param xMax Ascissa del bordo destro.
 * @param yMax Ordinata del bordo superiore.
 * @param color Colore RGB dei vertici.
 *
 * Aggiunge un rettangolo delimitato dalle coordinate minime e massime.
 */
void addRectangle(MeshData& mesh, float xMin, float yMin,
                  float xMax, float yMax, const glm::vec3& color)
{
    addQuad(mesh, {xMin, yMin}, {xMax, yMin}, {xMax, yMax}, {xMin, yMax}, color);
}

}

namespace DecorativeShapes {

MeshData rectangle(float width, float height, const glm::vec3& color)
{
    const float halfWidth = width * 0.5f;
    const float halfHeight = height * 0.5f;
    MeshData mesh;
    addRectangle(mesh, -halfWidth, -halfHeight, halfWidth, halfHeight, color);
    return mesh;
}

CharacterMeshes littlePrinceParts()
{
    CharacterMeshes parts;
    const glm::vec3 navy{0.08f, 0.15f, 0.28f};
    const glm::vec3 cape{0.14f, 0.36f, 0.66f};
    const glm::vec3 coat{0.31f, 0.66f, 0.93f};
    const glm::vec3 trim{0.68f, 0.88f, 1.00f};
    const glm::vec3 scarf{0.43f, 0.84f, 0.95f};
    const glm::vec3 skin{0.98f, 0.77f, 0.58f};
    const glm::vec3 hair{0.96f, 0.73f, 0.34f};
    const glm::vec3 gold{1.00f, 0.88f, 0.53f};

    addQuad(parts.body, {-0.44f, -0.43f}, {0.39f, -0.40f},
            {0.23f, 0.15f}, {-0.19f, 0.15f}, cape);
    addRectangle(parts.body, -0.20f, -0.50f, -0.05f, -0.25f, navy);
    addRectangle(parts.body, 0.05f, -0.50f, 0.20f, -0.25f, navy);

    addQuad(parts.body, {-0.28f, -0.35f}, {0.28f, -0.35f},
            {0.19f, 0.13f}, {-0.19f, 0.13f}, coat);
    addQuad(parts.body, {-0.35f, -0.16f}, {-0.23f, -0.19f},
            {-0.13f, 0.10f}, {-0.24f, 0.11f}, coat);
    addQuad(parts.body, {0.23f, -0.19f}, {0.35f, -0.16f},
            {0.24f, 0.11f}, {0.13f, 0.10f}, coat);
    addRectangle(parts.body, -0.35f, -0.20f, -0.25f, -0.16f, skin);
    addRectangle(parts.body, 0.25f, -0.20f, 0.35f, -0.16f, skin);
    addRectangle(parts.body, -0.24f, -0.24f, 0.24f, -0.19f, cape);
    addRectangle(parts.body, -0.04f, -0.245f, 0.04f, -0.18f, gold);

    addQuad(parts.head, {-0.14f, 0.14f}, {0.14f, 0.14f},
            {0.20f, 0.40f}, {-0.20f, 0.40f}, skin);
    addQuad(parts.head, {-0.23f, 0.29f}, {0.21f, 0.40f},
            {0.08f, 0.49f}, {-0.14f, 0.47f}, hair);
    parts.head.addTriangle({-0.18f, 0.41f}, {-0.02f, 0.50f}, {-0.02f, 0.42f}, gold);
    parts.head.addTriangle({0.04f, 0.43f}, {0.20f, 0.50f}, {0.15f, 0.41f}, hair);
    addRectangle(parts.head, -0.12f, 0.25f, -0.08f, 0.30f, navy);
    addRectangle(parts.head, 0.08f, 0.25f, 0.12f, 0.30f, navy);

    addQuad(parts.foreground, {0.11f, 0.04f}, {0.44f, 0.00f},
            {0.50f, 0.11f}, {0.16f, 0.15f}, scarf);
    addRectangle(parts.foreground, -0.19f, 0.06f, 0.18f, 0.15f, trim);
    return parts;
}

CharacterMeshes foxParts()
{
    CharacterMeshes parts;
    const glm::vec3 dark{0.23f, 0.09f, 0.12f};
    const glm::vec3 rust{0.77f, 0.25f, 0.13f};
    const glm::vec3 orange{0.96f, 0.45f, 0.19f};
    const glm::vec3 honey{1.00f, 0.61f, 0.29f};
    const glm::vec3 cream{1.00f, 0.88f, 0.71f};

    addQuad(parts.body, {0.12f, -0.38f}, {0.38f, -0.30f},
            {0.50f, -0.07f}, {0.28f, -0.13f}, rust);
    addQuad(parts.body, {0.28f, -0.13f}, {0.50f, -0.07f},
            {0.46f, 0.15f}, {0.34f, 0.08f}, orange);
    parts.body.addTriangle({0.36f, 0.00f}, {0.48f, 0.02f}, {0.46f, 0.15f}, cream);

    addQuad(parts.body, {-0.33f, -0.36f}, {0.21f, -0.36f},
            {0.18f, 0.09f}, {-0.25f, 0.09f}, rust);
    addQuad(parts.body, {-0.24f, -0.27f}, {0.07f, -0.30f},
            {0.12f, 0.06f}, {-0.20f, 0.06f}, orange);
    parts.body.addTriangle({-0.22f, 0.06f}, {-0.06f, -0.26f}, {0.10f, 0.06f}, cream);
    addRectangle(parts.body, -0.29f, -0.50f, -0.14f, -0.34f, dark);
    addRectangle(parts.body, 0.02f, -0.50f, 0.17f, -0.34f, dark);

    parts.head.addTriangle({-0.37f, 0.19f}, {-0.34f, 0.50f}, {-0.12f, 0.30f}, rust);
    parts.head.addTriangle({0.03f, 0.30f}, {0.24f, 0.50f}, {0.27f, 0.19f}, rust);
    parts.head.addTriangle({-0.32f, 0.24f}, {-0.31f, 0.42f}, {-0.21f, 0.29f}, cream);
    parts.head.addTriangle({0.12f, 0.29f}, {0.22f, 0.42f}, {0.23f, 0.24f}, cream);

    addQuad(parts.head, {-0.45f, 0.13f}, {0.35f, 0.13f},
            {0.17f, 0.32f}, {-0.27f, 0.32f}, orange);
    parts.head.addTriangle({-0.34f, 0.24f}, {-0.05f, 0.34f}, {0.23f, 0.24f}, honey);
    addQuad(parts.head, {-0.45f, 0.13f}, {-0.18f, -0.05f},
            {0.08f, -0.05f}, {0.35f, 0.13f}, cream);
    parts.head.addTriangle({-0.20f, 0.15f}, {-0.05f, 0.01f}, {0.10f, 0.15f}, orange);
    addRectangle(parts.head, -0.24f, 0.15f, -0.18f, 0.21f, dark);
    addRectangle(parts.head, 0.08f, 0.15f, 0.14f, 0.21f, dark);
    parts.head.addTriangle({-0.11f, 0.03f}, {0.01f, 0.03f}, {-0.05f, -0.03f}, dark);
    return parts;
}

namespace {

/**
 * @param parts Mesh dei diversi strati della figura.
 * @return Mesh che contiene tutti gli strati.
 *
 * Accoda al corpo i triangoli della testa e dei dettagli in primo piano.
 */
MeshData combine(CharacterMeshes parts)
{
    auto& vertices = parts.body.vertices;
    vertices.reserve(vertices.size() + parts.head.vertices.size() + parts.foreground.vertices.size());
    for (const auto* layer : {&parts.head, &parts.foreground}) {
        vertices.insert(vertices.end(), layer->vertices.begin(), layer->vertices.end());
    }
    return std::move(parts.body);
}

}

MeshData littlePrince() { return combine(littlePrinceParts()); }
MeshData fox() { return combine(foxParts()); }

namespace {

constexpr float bottom = -0.5f;
constexpr float archBase = 0.17f;
constexpr int archStones = 10;
constexpr glm::vec2 outerArchRadii{0.50f, 0.33f};
constexpr glm::vec2 innerArchRadii{0.38f, 0.24f};

/**
 * @param angle Angolo in radianti misurato dal lato destro dell'arco.
 * @param radii Semiassi orizzontale e verticale dell'ellisse.
 * @return Punto sul profilo dell'arco nelle coordinate locali della porta.
 */
glm::vec2 archPoint(float angle, const glm::vec2& radii)
{
    return {radii.x * std::cos(angle), archBase + radii.y * std::sin(angle)};
}

/**
 * @return Mesh del vano della porta.
 *
 * Genera il vano scuro che si vede tra le pietre della cornice.
 */
MeshData makeDoorRecess()
{
    MeshData mesh;
    const glm::vec3 mortar{0.20f, 0.19f, 0.24f};
    addRectangle(mesh, -0.50f, bottom, 0.50f, archBase, mortar);
    for (int i = 0; i < archStones; ++i) {
        const float start = glm::pi<float>() * i / archStones;
        const float end = glm::pi<float>() * (i + 1) / archStones;
        mesh.addTriangle({0.0f, archBase}, archPoint(start, outerArchRadii),
                         archPoint(end, outerArchRadii), mortar);
    }
    return mesh;
}

/**
 * @return Mesh della cornice.
 *
 * Genera i montanti, i conci dell'arco e la soglia della cornice in pietra.
 */
MeshData makeDoorFrame()
{
    MeshData mesh;
    const glm::vec3 stoneLight{0.55f, 0.51f, 0.53f};
    const glm::vec3 stoneDark{0.43f, 0.40f, 0.45f};
    for (int i = 0; i < 4; ++i) {
        const float y = bottom + (archBase - bottom) * i / 4;
        const float top = bottom + (archBase - bottom) * (i + 1) / 4 - 0.010f;
        const glm::vec3& stone = i % 2 == 0 ? stoneLight : stoneDark;
        addRectangle(mesh, -0.50f, y, -0.38f, top, stone);
        addRectangle(mesh, 0.38f, y, 0.50f, top, stone);
    }
    for (int i = 0; i < archStones; ++i) {
        const float start = glm::pi<float>() * i / archStones + 0.012f;
        const float end = glm::pi<float>() * (i + 1) / archStones - 0.012f;
        const glm::vec3& stone = i % 2 == 0 ? stoneLight : stoneDark;
        addQuad(mesh, archPoint(start, innerArchRadii), archPoint(start, outerArchRadii),
                archPoint(end, outerArchRadii), archPoint(end, innerArchRadii), stone);
    }

    addRectangle(mesh, -0.50f, bottom, 0.50f, -0.465f, stoneDark);
    return mesh;
}

/**
 * @param side Indice dell'anta, zero per la sinistra e uno per la destra.
 * @return Mesh di una singola anta.
 *
 * Genera un'anta con doghe e dettagli metallici nel riferimento locale della porta.
 */
MeshData makeDoorLeaf(std::size_t side)
{
    MeshData leaf;
    const bool left = side == 0;
    const float hinge = DoorMeshes::hingeX[side];
    const glm::vec3 wood{0.43f, 0.23f, 0.13f};
    const glm::vec3 woodShade{0.31f, 0.15f, 0.09f};
    const glm::vec3 iron{0.19f, 0.19f, 0.23f};
    const glm::vec3 brass{0.93f, 0.70f, 0.31f};

    const float minX = left ? hinge : 0.0f;
    const float maxX = left ? 0.0f : hinge;
    addRectangle(leaf, minX, bottom, maxX, archBase, wood);

    const int firstSegment = left ? archStones / 2 : 0;
    for (int i = firstSegment; i < firstSegment + archStones / 2; ++i) {
        const float start = glm::pi<float>() * i / archStones;
        const float end = glm::pi<float>() * (i + 1) / archStones;
        leaf.addTriangle({0.0f, archBase}, archPoint(start, innerArchRadii),
                         archPoint(end, innerArchRadii), wood);
    }
    for (float distance : {0.123f, 0.246f}) {
        const float x = left ? -distance : distance;
        const float ratio = (distance + 0.005f) / innerArchRadii.x;
        const float top = archBase + innerArchRadii.y * std::sqrt(1.0f - ratio * ratio) - 0.006f;
        addRectangle(leaf, x - 0.005f, bottom, x + 0.005f, top, woodShade);
    }
    addRectangle(leaf, left ? -0.012f : 0.0f, bottom,
                 left ? 0.0f : 0.012f, archBase + 0.233f, woodShade);

    for (float y : {-0.34f, 0.04f}) {
        addRectangle(leaf, left ? -0.37f : 0.09f, y,
                     left ? -0.09f : 0.37f, y + 0.045f, iron);
        addRectangle(leaf, left ? -0.32f : 0.30f, y + 0.013f,
                     left ? -0.30f : 0.32f, y + 0.032f, brass);
    }
    addRectangle(leaf, left ? -0.105f : 0.035f, -0.19f,
                 left ? -0.035f : 0.105f, -0.075f, iron);
    addRectangle(leaf, left ? -0.092f : 0.048f, -0.146f,
                 left ? -0.048f : 0.092f, -0.114f, brass);
    return leaf;
}

}

DoorMeshes door()
{
    DoorMeshes meshes;
    meshes.recess = makeDoorRecess();
    meshes.frame = makeDoorFrame();
    for (std::size_t side = 0; side < meshes.leaves.size(); ++side) {
        meshes.leaves[side] = makeDoorLeaf(side);
    }
    return meshes;
}

PressurePlateMeshes pressurePlate()
{
    PressurePlateMeshes meshes;
    const glm::vec3 housing{0.20f, 0.23f, 0.34f};
    const glm::vec3 steel{0.64f, 0.70f, 0.80f};
    const glm::vec3 bevel{0.82f, 0.88f, 0.94f};

    addQuad(meshes.base, {-0.88f, -0.50f}, {0.88f, -0.50f},
            {0.77f, 0.15f}, {-0.77f, 0.15f}, housing);
    addRectangle(meshes.base, -0.80f, -0.39f, 0.80f, -0.08f, steel);
    addRectangle(meshes.base, -0.77f, -0.08f, 0.77f, 0.15f, bevel);

    const auto makeButton = [](const glm::vec3& body, const glm::vec3& light) {
        MeshData mesh;
        addQuad(mesh, {-0.64f, -0.12f}, {0.64f, -0.12f},
                {0.57f, 0.50f}, {-0.57f, 0.50f}, body);
        addQuad(mesh, {-0.59f, 0.24f}, {0.59f, 0.24f},
                {0.57f, 0.50f}, {-0.57f, 0.50f}, light);
        return mesh;
    };

    meshes.releasedPlate = makeButton({0.74f, 0.14f, 0.21f}, {1.0f, 0.43f, 0.46f});
    meshes.pressedPlate = makeButton({0.10f, 0.62f, 0.33f}, {0.41f, 1.0f, 0.58f});
    return meshes;
}

MeshData crate()
{
    MeshData mesh;
    const glm::vec3 edge{0.22f, 0.12f, 0.08f};
    const glm::vec3 plank{0.56f, 0.34f, 0.17f};
    const glm::vec3 darkPlank{0.47f, 0.27f, 0.13f};
    const glm::vec3 brace{0.69f, 0.45f, 0.23f};
    const glm::vec3 iron{0.30f, 0.31f, 0.34f};

    addRectangle(mesh, -0.50f, -0.50f, 0.50f, 0.50f, edge);
    for (int i = 0; i < 4; ++i) {
        const float x = -0.39f + 0.20f * i;
        addRectangle(mesh, x, -0.39f, x + 0.18f, 0.39f,
                     i % 2 == 0 ? plank : darkPlank);
    }

    addQuad(mesh, {-0.40f, -0.39f}, {-0.22f, -0.39f},
            {0.40f, 0.39f}, {0.22f, 0.39f}, brace);
    for (float y : {-0.44f, 0.39f}) {
        addRectangle(mesh, -0.45f, y, 0.45f, y + 0.05f, brace);
        for (float x : {-0.39f, 0.35f}) {
            addRectangle(mesh, x, y + 0.01f, x + 0.04f, y + 0.04f, iron);
        }
    }
    return mesh;
}

MeshData movingWall()
{
    return rectangle(1.0f, 1.0f, {1.0f, 1.0f, 1.0f});
}

}
