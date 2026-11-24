#include "KeysInput.h"
#include <GLFW/glfw3.h>

namespace {

struct PlayerKeys {
    int left;
    int right;
    int jump;
    int down;
};

constexpr PlayerKeys firstPlayerKeys{GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S};
constexpr PlayerKeys secondPlayerKeys{GLFW_KEY_LEFT, GLFW_KEY_RIGHT, GLFW_KEY_UP, GLFW_KEY_DOWN};

/**
 * @param window Finestra GLFW del gioco.
 * @param keys Associazione dei tasti ai comandi del giocatore.
 * @return Stato dei comandi per il passo corrente.
 *
 * Legge lo stato dei quattro tasti associati al giocatore, incluso il salto tenuto.
 */
PlayerInput readPlayerInput(GLFWwindow* window, const PlayerKeys& keys)
{
    return {
        glfwGetKey(window, keys.left) == GLFW_PRESS,
        glfwGetKey(window, keys.right) == GLFW_PRESS,
        glfwGetKey(window, keys.jump) == GLFW_PRESS,
        glfwGetKey(window, keys.down) == GLFW_PRESS
    };
}

}

KeyboardState KeysInput::read(GLFWwindow* window)
{
    const bool escapeDown = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
    const bool restartDown = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
    const bool escapeJustPressed = escapeDown && !m_escapeWasDown;
    const bool restartJustPressed = restartDown && !m_restartWasDown;
    m_escapeWasDown = escapeDown;
    m_restartWasDown = restartDown;

    return {
        readPlayerInput(window, firstPlayerKeys),
        readPlayerInput(window, secondPlayerKeys),
        escapeJustPressed,
        restartJustPressed
    };
}
