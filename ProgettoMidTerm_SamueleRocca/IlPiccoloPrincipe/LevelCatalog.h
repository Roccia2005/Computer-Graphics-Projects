#pragma once
#include <cstddef>

struct LevelDefinition;

namespace Levels {

/**
 * @param index Indice dell'elemento, a partire da zero.
 * @return Definizione del livello richiesto.
 *
 * Cerca la stanza nel catalogo. Un indice fuori intervallo genera std::out_of_range.
 */
const LevelDefinition& get(std::size_t index);
/**
 * @return Numero delle stanze della campagna.
 *
 * Restituisce il numero di livelli disponibili nel catalogo.
 */
std::size_t count();
/**
 * @return Definizione del tutorial in sola lettura.
 *
 * Definizione della stanza di prova accessibile dal menu.
 */
const LevelDefinition& tutorial();

}
