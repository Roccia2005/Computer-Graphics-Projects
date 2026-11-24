#pragma once
#include "MeshData.h"
#include <filesystem>

namespace ObjLoader {
    /**
     * @param path Percorso del file da leggere.
     * @return Mesh triangolata nelle coordinate XY del file OBJ e i colori dei materiali.
     *
     * Legge le facce del modello OBJ, le divide in triangoli e assegna i colori dei materiali MTL.
     */
    MeshData load(const std::filesystem::path& path);
}
