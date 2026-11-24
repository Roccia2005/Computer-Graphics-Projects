#pragma once
#include "MeshData.h"
#include <glm/glm.hpp>
#include <array>
namespace DecorativeShapes {
/**
 * @param width Larghezza del rettangolo.
 * @param height Altezza del rettangolo.
 * @param color Colore RGB dei vertici.
 * @return Mesh di un rettangolo centrato nell'origine, composto da due triangoli dello stesso colore.
 */
MeshData rectangle(float width, float height, const glm::vec3& color);

struct CharacterMeshes {
    MeshData body;
    MeshData head;
    MeshData foreground;
};
/**
 * @return Parti del Principe nel medesimo riferimento locale.
 *
 * Compone il Piccolo Principe con il cappotto azzurro, il mantello e la sciarpa. La base degli
 * stivali a y = -0.5 coincide con la quota dei piedi.
 */
CharacterMeshes littlePrinceParts();
/**
 * @return Parti della Volpe nel medesimo riferimento locale.
 *
 * Compone la Volpe con orecchie a punta, muso chiaro e coda folta. La base delle zampe ha la
 * stessa quota locale degli stivali del Principe.
 */
CharacterMeshes foxParts();
/**
 * @return Mesh completa del Principe.
 *
 * Unisce corpo, testa e dettagli del Principe in una sola mesh per il livello.
 */
MeshData littlePrince();
/**
 * @return Mesh completa della Volpe.
 *
 * Unisce le parti della Volpe in una sola mesh per il livello.
 */
MeshData fox();
/**
 * @return Mesh della cassa.
 *
 * Cassa in legno con assi verticali, rinforzo diagonale e chiodi.
 */
MeshData crate();
/**
 * @return Rettangolo unitario per lo shader della parete.
 *
 * Il materiale del muro viene disegnato sul rettangolo unitario.
 */
MeshData movingWall();
struct DoorMeshes {
    MeshData recess;
    std::array<MeshData, 2> leaves;
    MeshData frame;
    static constexpr std::array<float, 2> hingeX{-0.38f, 0.38f};
};
/**
 * @return Vano, ante e cornice della porta.
 *
 * Restituisce le mesh del vano, della cornice e delle due ante nello stesso riferimento locale.
 * Le ante sono separate per consentire trasformazioni indipendenti attorno ai cardini.
 */
DoorMeshes door();

struct PressurePlateMeshes {
    MeshData base;
    MeshData releasedPlate;
    MeshData pressedPlate;
};
/**
 * @return Base e due varianti del pulsante.
 *
 * Restituisce la base della pedana e due varianti cromatiche del pulsante. Lo spostamento
 * verticale del pulsante viene applicato dalla vista.
 */
PressurePlateMeshes pressurePlate();
}
