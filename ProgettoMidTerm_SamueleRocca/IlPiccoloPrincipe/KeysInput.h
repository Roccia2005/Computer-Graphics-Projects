#pragma once
#include "LevelTypes.h"

struct GLFWwindow;

struct KeyboardState {
    PlayerInput firstPlayer;
    PlayerInput secondPlayer;
    bool escapeJustPressed;
    bool restartJustPressed;
};

class KeysInput {
public:
    /**
     * @param window Finestra da cui leggere lo stato della tastiera.
     * @return Comandi dei giocatori e nuove pressioni di Esc e R.
     *
     * Memorizza lo stato di Esc e R per riconoscere la pressione successiva.
     */
    KeyboardState read(GLFWwindow* window);

private:
    bool m_escapeWasDown = false;
    bool m_restartWasDown = false;
};
