#include "componentdisplay.h"

VertexDisplay::VertexDisplay(OpenGLContext *context)
    : Drawable(context), vertex(nullptr)
{}

GLenum VertexDisplay::drawMode() const
{
    return GL_POINTS;
}

void VertexDisplay::initializeAndBufferGeometryData()
{
    indexBufferLength = 0;
    if(!vertex) {
        return;
    }
    
    std::vector<glm::vec3> positions {vertex->position};
    std::vector<glm::vec3> colors {glm::vec3(1.f)};
    std::vector<GLuint> indices {0};
    indexBufferLength = 1;
    generateBuffer(INDEX);
    bindBuffer(INDEX);
    bufferData(INDEX, indices);
    generateBuffer(POSITION);
    bindBuffer(POSITION);
    bufferData(POSITION, positions);
    generateBuffer(COLOR);
    bindBuffer(COLOR);
    bufferData(COLOR, colors);
}

HalfEdgeDisplay::HalfEdgeDisplay(OpenGLContext *context)
    : Drawable(context), edge(nullptr)
{}

GLenum HalfEdgeDisplay::drawMode() const
{
    return GL_LINES;
}

void HalfEdgeDisplay::initializeAndBufferGeometryData()
{
    indexBufferLength = 0;
    if(!edge) {
        return;
    }
    
    HalfEdge *previous = edge;
    while(previous->next != edge) {
        previous = previous->next;
    }
    
    std::vector<glm::vec3> positions {previous->vertex->position, edge->vertex->position};
    std::vector<glm::vec3> colors {glm::vec3(1, 0, 0), glm::vec3(1, 1, 0)};
    std::vector<GLuint> indices {0, 1};
    indexBufferLength = 2;
    generateBuffer(INDEX);
    bindBuffer(INDEX);
    bufferData(INDEX, indices);
    generateBuffer(POSITION);
    bindBuffer(POSITION);
    bufferData(POSITION, positions);
    generateBuffer(COLOR);
    bindBuffer(COLOR);
    bufferData(COLOR, colors);
}

FaceDisplay::FaceDisplay(OpenGLContext *context)
    : Drawable(context), face(nullptr)
{}

GLenum FaceDisplay::drawMode() const
{
    return GL_LINES;
}

void FaceDisplay::initializeAndBufferGeometryData()
{
    indexBufferLength = 0;
    if(!face) {
        return;
    }
    
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> colors;
    std::vector<GLuint> indices;
    HalfEdge *edge = face->edge;
    
    do {
        positions.push_back(edge->vertex->position);
        colors.push_back(glm::vec3(1.f) - face->color);
        edge = edge->next;
    } while(edge != face->edge);
    
    for(GLuint i = 0; i < positions.size(); ++i) {
        indices.push_back(i);
        indices.push_back((i + 1) % positions.size());
    }
    
    indexBufferLength = indices.size();
    generateBuffer(INDEX);
    bindBuffer(INDEX);
    bufferData(INDEX, indices);
    generateBuffer(POSITION);
    bindBuffer(POSITION);
    bufferData(POSITION, positions);
    generateBuffer(COLOR);
    bindBuffer(COLOR);
    bufferData(COLOR, colors);
}