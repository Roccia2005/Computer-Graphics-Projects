#include "Geometry.h"
Geometry::Geometry(const MeshData& mesh) {
    constexpr GLsizei vertexStrideBytes = MeshData::floatsPerVertex * sizeof(float);
    constexpr auto colorOffsetBytes = MeshData::positionComponents * sizeof(float);
    m_vertexCount = static_cast<GLsizei>(mesh.vertices.size() / MeshData::floatsPerVertex);
    m_primitive=mesh.primitive;
    glGenVertexArrays(1,&m_vao);
    glGenBuffers(1,&m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
    glBufferData(GL_ARRAY_BUFFER,mesh.vertices.size()*sizeof(float),mesh.vertices.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0, MeshData::positionComponents, GL_FLOAT, GL_FALSE, vertexStrideBytes, nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, MeshData::colorComponents, GL_FLOAT, GL_FALSE, vertexStrideBytes,
                          reinterpret_cast<const void*>(colorOffsetBytes));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}
Geometry::~Geometry() {
    if(m_vbo) glDeleteBuffers(1,&m_vbo);
    if(m_vao) glDeleteVertexArrays(1,&m_vao);
}
void Geometry::draw() const {
    glBindVertexArray(m_vao);
    glDrawArrays(m_primitive,0,m_vertexCount);
}
