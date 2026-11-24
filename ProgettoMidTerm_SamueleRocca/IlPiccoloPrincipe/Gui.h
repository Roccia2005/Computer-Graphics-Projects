#pragma once

struct GLFWwindow;
enum class UiAction;

class Gui
{
public:
    /**
     * @param window Finestra GLFW del gioco.
     *
     * Crea il contesto ImGui e inizializza i backend GLFW e OpenGL 3.3 per la finestra ricevuta.
     */
    explicit Gui(GLFWwindow* window);
    /**
     * Disattiva i backend e distrugge il contesto ImGui prima della chiusura della finestra.
     */
    ~Gui();
    /**
     * Vieta la costruzione per copia per mantenere un unico proprietario delle risorse.
     */
    Gui(const Gui&) = delete;
    /**
     * @return Nessun valore ottenibile, poiché l'operatore è eliminato.
     *
     * Vieta l'assegnazione per copia per mantenere un unico proprietario delle risorse.
     */
    Gui& operator=(const Gui&) = delete;
    /**
     * Avvia un frame ImGui dopo avere aggiornato i due backend. Chiamare prima di costruire i
     * widget.
     */
    void beginFrame();
    /**
     * @return Azione scelta, oppure UiAction::None.
     *
     * Disegna il menu e restituisce l'azione selezionata.
     */
    UiAction drawMenu();
    /**
     * Mostra i comandi mentre i due giocatori esplorano la stanza di prova.
     */
    void drawTutorialControls();
    /**
     * Conclude il frame ImGui e invia a OpenGL i comandi di disegno dei widget.
     */
    void render();

private:
    bool m_tutorialSelected = false;
};
