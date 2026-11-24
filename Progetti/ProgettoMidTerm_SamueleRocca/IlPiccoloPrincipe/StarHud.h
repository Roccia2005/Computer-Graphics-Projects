#pragma once
#include <glm/glm.hpp>
#include <optional>

struct StarProgress;

namespace StarHud {

/**
 * @param stars Stato della raccolta e dello sblocco dell'uscita.
 * @param remainingSeconds Secondi residui del tentativo, assenti nelle stanze senza limite.
 * @param doorTopInWindow Centro del bordo superiore della porta in coordinate logiche della finestra.
 * @param worldTime Tempo della stanza in secondi.
 *
 * Mostra le stelle presenti, gli avvisi della porta e il timer con allerta negli ultimi dieci secondi.
 */
void draw(const StarProgress& stars, std::optional<float> remainingSeconds,
          glm::vec2 doorTopInWindow, float worldTime);

}
