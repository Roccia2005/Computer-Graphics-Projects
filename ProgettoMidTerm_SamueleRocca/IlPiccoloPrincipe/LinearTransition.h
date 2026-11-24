#pragma once
#include <algorithm>

class LinearTransition {
public:
    /**
     * @return Frazione del percorso tra zero e uno.
     *
     * Legge la posizione corrente tra gli estremi della transizione.
     */
    float progress() const { return m_progress; }

    /**
     * @param dt Intervallo di simulazione in secondi.
     * @param towardEnd true per avanzare verso uno, false per tornare a zero.
     * @param duration Durata complessiva in secondi.
     *
     * Avanza o arretra in proporzione al tempo trascorso e limita il progresso tra zero e uno.
     */
    void update(float dt, bool towardEnd, float duration)
    {
        if (dt <= 0.0f) {
            return;
        }
        const float step = duration > 0.0f ? dt / duration : 1.0f;
        m_progress = std::clamp(m_progress + (towardEnd ? step : -step), 0.0f, 1.0f);
    }

    /**
     * Riporta il progresso all'inizio della transizione.
     */
    void reset() { m_progress = 0.0f; }

private:
    float m_progress = 0.0f;
};
