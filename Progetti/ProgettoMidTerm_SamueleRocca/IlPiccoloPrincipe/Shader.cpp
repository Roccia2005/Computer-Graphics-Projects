#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
namespace {
/**
 * @param path Percorso del file GLSL.
 * @return Testo del sorgente da compilare.
 *
 * Legge il sorgente e segnala il percorso se il file risulta vuoto o illeggibile.
 */
std::string readShaderSource(const std::string& path)
{
    std::ifstream file(path);
    if (!file) throw std::runtime_error("Impossibile aprire il file shader: " + path);

    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    if (source.empty()) throw std::runtime_error("File shader vuoto: " + path);
    if (!buffer) throw std::runtime_error("Lettura del file shader fallita: " + path);
    return source;
}

/**
 * @param kind Stadio OpenGL da compilare.
 * @param src Sorgente GLSL da compilare.
 * @return Identificatore OpenGL dello shader compilato.
 *
 * Compila il sorgente GLSL per lo stadio kind e restituisce l'handle dello shader. Se fallisce,
 * stampa il log, elimina lo shader e lancia std::runtime_error.
 */
GLuint compileShader(GLenum kind,const std::string& src) {
    GLuint id=glCreateShader(kind);
    const char* ptr=src.c_str();
    glShaderSource(id,1,&ptr,nullptr);
    glCompileShader(id);
    GLint ok=0;glGetShaderiv(id,GL_COMPILE_STATUS,&ok);
    if(!ok) {
        char log[4096]={};glGetShaderInfoLog(id,sizeof(log),nullptr,log);
        glDeleteShader(id);
        std::cerr << log << '\n';
        throw std::runtime_error("Compilazione shader fallita");
    }
    return id;
}
}
Shader Shader::fromFiles(const std::string& vertexPath, const std::string& fragmentPath)
{
    const auto vertexSource = readShaderSource(vertexPath);
    const auto fragmentSource = readShaderSource(fragmentPath);
    return Shader(vertexSource, fragmentSource);
}

Shader::Shader(const std::string& vs,const std::string& fs) {
    const GLuint vertex = compileShader(GL_VERTEX_SHADER, vs);
    GLuint fragment;
    try {
        fragment = compileShader(GL_FRAGMENT_SHADER, fs);
    } catch (...) {
        glDeleteShader(vertex);
        throw;
    }

    m_program = glCreateProgram();
    glAttachShader(m_program, vertex);
    glAttachShader(m_program, fragment);
    glLinkProgram(m_program);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    GLint linked = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char log[4096] = {};
        glGetProgramInfoLog(m_program, sizeof(log), nullptr, log);
        glDeleteProgram(m_program);
        std::cerr << log << '\n';
        throw std::runtime_error("Link shader fallito");
    }
}
Shader::~Shader() {if(m_program)glDeleteProgram(m_program);}
void Shader::use() const {glUseProgram(m_program);}
/**
 * @param name Nome della uniform nel programma GLSL.
 * @return Posizione della uniform, oppure -1 se assente.
 *
 * Cerca la posizione della uniform e la memorizza per gli accessi successivi.
 */
GLint Shader::uniformLocation(const char* name) const {
    const auto [entry, inserted] = m_uniformLocations.try_emplace(name, -1);
    if (inserted) {
        entry->second = glGetUniformLocation(m_program, name);
    }
    return entry->second;
}

void Shader::setMat4(const char* name,const glm::mat4& value) const {
    glUniformMatrix4fv(uniformLocation(name),1,GL_FALSE,glm::value_ptr(value));
}

void Shader::setFloat(const char* name, float value) const {
    glUniform1f(uniformLocation(name), value);
}
