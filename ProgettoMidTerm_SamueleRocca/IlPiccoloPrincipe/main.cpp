#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Config.h"
#include "LevelView.h"
#include "DialogueView.h"
#include "Shader.h"
#include "Renderer.h"
#include "BackgroundRenderer.h"
#include "StarRenderer.h"
#include "GameFlow.h"
#include "Gui.h"
#include "DialoguePanel.h"
#include "StarHud.h"
#include "KeysInput.h"
#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>


namespace {

class GlfwSession {
public:
    /**
     * Inizializza GLFW e segnala con un'eccezione un eventuale errore.
     */
    GlfwSession()
    {
        if (!glfwInit()) throw std::runtime_error("Errore inizializzazione GLFW");
    }
    /**
     * Termina GLFW alla fine della sessione.
     */
    ~GlfwSession() { glfwTerminate(); }

    /**
     * Vieta la costruzione per copia per mantenere un unico proprietario delle risorse.
     */
    GlfwSession(const GlfwSession&) = delete;
    /**
     * @return Nessun valore ottenibile, poiché l'operatore è eliminato.
     *
     * Vieta l'assegnazione per copia per mantenere un unico proprietario delle risorse.
     */
    GlfwSession& operator=(const GlfwSession&) = delete;
};

/**
 * @param screen Schermata corrente del gioco.
 * @return true in campagna e nel tutorial.
 *
 * Identifica le schermate in cui i comandi controllano i personaggi.
 */
bool isPlayable(Screen screen)
{
    return screen == Screen::Story || screen == Screen::Tutorial;
}

using WindowOwner = std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>;

/**
 * @return Finestra del gioco con il contesto OpenGL corrente.
 *
 * Crea la finestra, attiva la sincronizzazione verticale e carica le funzioni OpenGL.
 * Se la creazione della finestra o il caricamento di GLAD fallisce, segnala un errore.
 */
WindowOwner createGameWindow()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    WindowOwner window(glfwCreateWindow(1100, 700, GameConfig::GAME_TITLE, nullptr, nullptr),
                       glfwDestroyWindow);
    if (!window) throw std::runtime_error("Creazione della finestra fallita");
    glfwMakeContextCurrent(window.get());
    glfwSwapInterval(1);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        throw std::runtime_error("Errore inizializzazione GLAD");
    }
    return window;
}

/**
 * @param background Renderer dello sfondo animato.
 * @param timeSeconds Tempo corrente dell'animazione o della simulazione, in secondi.
 *
 * Pulisce il buffer dei colori e disegna lo sfondo animato.
 */
void drawBackground(const BackgroundRenderer& background, float timeSeconds)
{
    glClearColor(0.06f, 0.11f, 0.20f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    background.draw(timeSeconds);
}

/**
 * @param width Larghezza del framebuffer in pixel.
 * @param height Altezza del framebuffer in pixel.
 * @param flow Stato delle schermate e della partita.
 * @param levelView Vista che sincronizza le istanze della stanza.
 * @param renderer Renderer degli oggetti della scena.
 * @param background Renderer dello sfondo animato.
 * @param starRenderer Renderer delle stelle animate.
 * @param timeSeconds Tempo corrente dell'animazione o della simulazione, in secondi.
 *
 * Disegna fondale e stanza prima dei pannelli dell'interfaccia.
 */
void renderWorld(int width, int height, const GameFlow& flow, LevelView& levelView,
                 Renderer& renderer, const BackgroundRenderer& background,
                 const StarRenderer& starRenderer, float timeSeconds)
{
    const Level* level = flow.levelToDraw();
    if (level) {
        levelView.sync(*level, flow.levelNumber(), flow.worldTime());
        renderer.setRoomViewport(width, height, level->roomHalfSize());
    } else {
        glViewport(0, 0, width, height);
    }

    drawBackground(background, timeSeconds);
    if (level) {
        renderer.drawScene(levelView.scene());
        starRenderer.draw(levelView.scene(), renderer.projectionMatrix(), flow.worldTime());
        renderer.drawScene(levelView.scene(), SceneLayer::Foreground);
    }
}

/**
 * @param window Finestra GLFW del gioco.
 * @param flow Stato da cui ricavare schermata e numero del livello.
 * @param previousTitle Ultimo titolo impostato, aggiornato se il testo cambia.
 *
 * Mostra nel titolo il livello corrente e il comando di reset durante la campagna.
 */
void updateWindowTitle(GLFWwindow* window, const GameFlow& flow, std::string& previousTitle)
{
    std::string title = GameConfig::GAME_TITLE;
    if (flow.screen() == Screen::Story) {
        title += " | Livello " + std::to_string(flow.levelNumber()) + " | R reset";
    } else if (flow.screen() == Screen::Tutorial) {
        title += " | Tutorial";
    }
    if (title != previousTitle) {
        glfwSetWindowTitle(window, title.c_str());
        previousTitle = std::move(title);
    }
}

/**
 * @param line Battuta corrente.
 * @param width Larghezza del framebuffer in pixel.
 * @param height Altezza del framebuffer in pixel.
 * @param now Tempo corrente in secondi.
 * @param dialogueView Vista dei personaggi.
 * @param renderer Renderer delle figure.
 * @param background Renderer dello sfondo animato.
 * @return Azione scelta nel pannello, da applicare al fotogramma successivo.
 *
 * Prepara il pannello e disegna le figure nell'area riservata ai personaggi.
 */
UiAction renderDialogue(const DialogueLine& line, int width, int height, double now,
                        DialogueView& dialogueView, Renderer& renderer,
                        const BackgroundRenderer& background)
{
    const auto panel = DialoguePanel::draw(line);
    const glm::vec2 framebufferSize(width, height);
    renderer.setRoomViewport(width, height, framebufferSize * 0.5f);
    drawBackground(background, static_cast<float>(now));
    if (dialogueView.updatePose(panel, framebufferSize, now)) {
        renderer.drawScene(dialogueView.scene());
    }
    return panel.action;
}

/**
 * @param level Stanza da cui leggere porta, stelle raccolte e tempo residuo in secondi, se previsto.
 * @param window Finestra del gioco.
 * @param renderer Renderer che converte le coordinate della stanza.
 * @param framebufferSize Dimensioni del framebuffer in pixel.
 * @param worldTime Tempo della simulazione in secondi.
 *
 * Disegna l'HUD della Storia e converte la posizione della porta per l'avviso delle stelle mancanti.
 */
void drawLevelStarHud(const Level& level, GLFWwindow* window, const Renderer& renderer,
                      glm::ivec2 framebufferSize, float worldTime)
{
    const AABB& door = level.exitDoor().area();
    const glm::vec2 doorTop = door.center + glm::vec2{0.0f, door.halfSize.y};
    glm::ivec2 windowSize;
    glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
    const auto doorTopInWindow = renderer.windowPosition(doorTop, framebufferSize, windowSize);
    StarHud::draw(level.starProgress(), level.remainingSeconds(), doorTopInWindow, worldTime);
}

}

/**
 * @return Zero alla chiusura regolare, uno in caso di errore.
 *
 * Inizializza finestra, OpenGL e gioco. A ogni frame legge gli input, aggiorna la simulazione e
 * disegna la vista della stanza. Rilascia le risorse GPU prima di distruggere il contesto.
 */
int main()
{
    try {
        GlfwSession glfw;
        auto windowOwner = createGameWindow();
        GLFWwindow* window = windowOwner.get();

        auto basicShader = Shader::fromFiles("shaders/basic.vert", "shaders/basic.frag");
        auto wallShader = Shader::fromFiles("shaders/moving_wall.vert", "shaders/moving_wall.frag");
        Renderer renderer({
            {MaterialId::Basic, {basicShader}},
            {MaterialId::WallIdle, {wallShader, [](const Shader& s) { s.setFloat("uMoving", 0.0f); }}},
            {MaterialId::WallMoving, {wallShader, [](const Shader& s) { s.setFloat("uMoving", 1.0f); }}}
        });

        auto backgroundShader = Shader::fromFiles("shaders/background.vert", "shaders/background.frag");
        BackgroundRenderer background(backgroundShader);

        auto starShader = Shader::fromFiles("shaders/star.vert", "shaders/basic.frag");
        StarRenderer starRenderer(starShader);
        GameFlow flow;
        LevelView levelView;
        DialogueView dialogueView;
        Gui gui(window);
        std::string previousTitle = GameConfig::GAME_TITLE;

        KeysInput keyboard;
        UiAction pendingAction = UiAction::None;
        double previousTime = glfwGetTime();

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            const double now = glfwGetTime();
            const float dt = std::clamp(
                static_cast<float>(now - previousTime), 0.0f, GameConfig::MAX_FRAME_DELTA_SECONDS);
            previousTime = now;

            const Screen screen = flow.screen();
            const bool playing = isPlayable(screen);
            const KeyboardState keys = keyboard.read(window);

            UiAction action = std::exchange(pendingAction, UiAction::None);
            if (keys.escapeJustPressed) {
                if (screen == Screen::Tutorial) {
                    action = UiAction::BackToMenu;
                } else {
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                    break;
                }
            } else if (playing && keys.restartJustPressed) {
                action = UiAction::RestartRoom;
            }

            int width = 0, height = 0;
            glfwGetFramebufferSize(window, &width, &height);
            const bool minimized = width <= 0 || height <= 0 || glfwGetWindowAttrib(window, GLFW_ICONIFIED);
            if (action != UiAction::None) {
                flow.applyAction(action, static_cast<float>(now));
            } else if (playing && !minimized) {
                flow.update(dt, static_cast<float>(now), keys.firstPlayer, keys.secondPlayer);
            }

            const DialogueLine* line = flow.dialogueLine();
            dialogueView.observeLine(line);
            updateWindowTitle(window, flow, previousTitle);
            if (minimized) {
                continue;
            }

            gui.beginFrame();
            if (line) {
                pendingAction = renderDialogue(*line, width, height, now, dialogueView,
                                               renderer, background);
            } else {
                renderWorld(width, height, flow, levelView, renderer, background, starRenderer,
                            static_cast<float>(now));
                switch (flow.screen()) {
                case Screen::MainMenu:
                    pendingAction = gui.drawMenu();
                    break;

                case Screen::Tutorial:
                    gui.drawTutorialControls();
                    break;

                case Screen::Story:
                    drawLevelStarHud(*flow.levelToDraw(), window, renderer,
                                     {width, height}, flow.worldTime());
                    break;

                default:
                    break;
                }
            }
            gui.render();
            glfwSwapBuffers(window);
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }

    return 0;
}
