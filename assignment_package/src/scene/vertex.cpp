#include "vertex.h"

int Vertex::nextID = 0;

Vertex::Vertex(const glm::vec3 &position)
    : position(position), edge(nullptr), id(nextID++)
{
    setText(QString::number(id));
}