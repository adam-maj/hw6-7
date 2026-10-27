#pragma once

#include <QListWidgetItem>
#include <la.h>

class HalfEdge;

class Face : public QListWidgetItem {
public:
    HalfEdge *edge;
    glm::vec3 color;
    int id;
    static int nextID;

    Face();
};