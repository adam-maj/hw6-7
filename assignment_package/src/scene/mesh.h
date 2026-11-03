#pragma once

#include "drawable.h"
#include "vertex.h"
#include "face.h"
#include "halfedge.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <sstream>

class Mesh : public Drawable {
public:
    std::vector<std::unique_ptr<Vertex>> vertices;
    std::vector<std::unique_ptr<Face>> faces;
    std::vector<std::unique_ptr<HalfEdge>> halfEdges;

    Mesh(OpenGLContext *context);
    void createCube();
    bool loadOBJ(const QString &filename);
    void initializeAndBufferGeometryData() override;
    void splitEdge(HalfEdge *edge);
    void triangulateFace(Face *face);
    void subdivide();
    void extrudeFace(Face *face);

private:
    void build(const std::vector<glm::vec3> &positions,
               const std::vector<std::vector<int>> &polygons);
    std::vector<HalfEdge*> faceEdges(Face *face) const;
    void splitEdgeAt(HalfEdge *edge, const glm::vec3 &position);
    void createFacePoints(std::unordered_map<Face*, Vertex*> &facePoints);
    void computeEdgePoints(const std::unordered_map<Face*, Vertex*> &facePoints,
                        std::unordered_map<HalfEdge*, glm::vec3> &edgePoints);
    void smoothVertices(const std::unordered_map<Face*, Vertex*> &facePoints,
                        const std::unordered_map<HalfEdge*, glm::vec3> &edgePoints);
    void quadrangulateFace(Face *face, Vertex *center);
    bool readOBJFace(std::istringstream &stream, int vertexCount,
                 std::vector<int> &polygon);
};