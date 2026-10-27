#include "face.h"
#include <cstdlib>

int Face::nextID = 0;

Face::Face()
    : edge(nullptr),
      color(float(std::rand()) / RAND_MAX,
            float(std::rand()) / RAND_MAX,
            float(std::rand()) / RAND_MAX),
      id(nextID++)
{
    setText(QString::number(id));
}