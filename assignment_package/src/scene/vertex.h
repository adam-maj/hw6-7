#pragma once

#include <QListWidgetItem>
#include <la.h>

class HalfEdge;

class Vertex : public QListWidgetItem {
public:
    glm::vec3 position;
    HalfEdge *edge;
    int id;
    static int nextID;

    Vertex(const glm::vec3 &position);
};