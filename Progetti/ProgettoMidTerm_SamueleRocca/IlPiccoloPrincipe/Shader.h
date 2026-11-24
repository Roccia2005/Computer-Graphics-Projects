#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
class Shader {
    GLuint m_program=0;
    mutable std::unordered_map<std::string, GLint> m_uniformLocations;
    GLint uniformLocation(const char* name) const;
public:
    /**
     * @param vertexPath Percorso del sorgente GLSL di vertice.
     * @param fragmentPath Percorso del sorgente GLSL di frammento.
     * @return Programma OpenGL compilato e collegato.
     *
     * Legge i due file e segnala con un'eccezione il percorso di un sorgente vuoto o illeggibile.
     */
    static Shader fromFiles(const std::string& vertexPath, const std::string& fragmentPath);

    /**
     * @param vertexSource Sorgente GLSL dello shader di vertice.
     * @param fragmentSource Sorgente GLSL dello shader di frammento.
     *
     * Compila i sorgenti vertex e fragment e li collega in un programma. Segnala gli errori di
     * compilazione o link con un'eccezione.
     */
    Shader(const std::string& vertexSource,const std::string& fragmentSource);
    /**
     * Rilascia il programma GLSL posseduto dall'oggetto mentre il contesto OpenGL è ancora attivo.
     */
    ~Shader();
    /**
     * Vieta la costruzione per copia per mantenere un unico proprietario delle risorse.
     */
    Shader(const Shader&)=delete;
    /**
     * @return Nessun valore ottenibile, poiché l'operatore è eliminato.
     *
     * Vieta l'assegnazione per copia per mantenere un unico proprietario delle risorse.
     */
    Shader& operator=(const Shader&)=delete;
    /**
     * Rende attivo il programma per le successive scritture delle uniform e chiamate di disegno.
     */
    void use() const;
    /**
     * @param uniformName Nome della variabile uniform nel programma GLSL.
     * @param value Matrice 4 × 4 da inviare allo shader.
     *
     * Aggiorna la matrice uniform del programma attivo.
     */
    void setMat4(const char* uniformName,const glm::mat4& value) const;
    /**
     * @param uniformName Nome della variabile uniform nel programma GLSL.
     * @param value Valore da assegnare alla uniform.
     *
     * Aggiorna la uniform scalare del programma attivo.
     */
    void setFloat(const char* uniformName, float value) const;
};
