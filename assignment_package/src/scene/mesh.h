#pragma once

#include "drawable.h"
#include "vertex.h"
#include "face.h"
#include "halfedge.h"
#include <memory>
#include <vector>

class Mesh : public Drawable {
public:
    std::vector<std::unique_ptr<Vertex>> vertices;
    std::vector<std::unique_ptr<Face>> faces;
    std::vector<std::unique_ptr<HalfEdge>> halfEdges;

    Mesh(OpenGLContext *context);
    void createCube();
    bool loadOBJ(const QString &filename);
    void initializeAndBufferGeometryData() override;

private:
    void build(const std::vector<glm::vec3> &positions,
               const std::vector<std::vector<int>> &polygons);
};