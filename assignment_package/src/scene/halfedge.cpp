#include "halfedge.h"

int HalfEdge::nextID = 0;

HalfEdge::HalfEdge()
    : next(nullptr), sym(nullptr), face(nullptr), vertex(nullptr), id(nextID++)
{
    setText(QString::number(id));
}