#include "mesh.h"
#include <fstream>
#include <sstream>
#include <map>

Mesh::Mesh(OpenGLContext *context) : Drawable(context)
{}

void Mesh::initializeAndBufferGeometryData()
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> colors;
    std::vector<GLuint> indices;

    for(const auto &face : faces) {
        GLuint first = positions.size();
        HalfEdge *edge = face->edge;
        glm::vec3 normal(0.f);

        do {
            normal += glm::cross(edge->vertex->position, edge->next->vertex->position);
            edge = edge->next;
        } while(edge != face->edge);

        if(glm::length(normal) > 0.f) {
            normal = glm::normalize(normal);
        }

        do {
            positions.push_back(edge->vertex->position);
            normals.push_back(normal);
            colors.push_back(face->color);
            edge = edge->next;
        } while(edge != face->edge);
        
        for(GLuint i = first + 1; i + 1 < positions.size(); ++i) {
            indices.push_back(first);
            indices.push_back(i);
            indices.push_back(i + 1);
        }
    }

    indexBufferLength = indices.size();
    generateBuffer(INDEX);
    bindBuffer(INDEX);
    bufferData(INDEX, indices);
    generateBuffer(POSITION);
    bindBuffer(POSITION);
    bufferData(POSITION, positions);
    generateBuffer(NORMAL);
    bindBuffer(NORMAL);
    bufferData(NORMAL, normals);
    generateBuffer(COLOR);
    bindBuffer(COLOR);
    bufferData(COLOR, colors);
}

void Mesh::createCube()
{
    std::vector<glm::vec3> positions {
        glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.5f, -0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, 0.5f, 0.5f)
    };

    std::vector<std::vector<int>> polygons {
        {0, 3, 2, 1}, {4, 5, 6, 7}, {0, 1, 5, 4},
        {3, 7, 6, 2}, {0, 4, 7, 3}, {1, 2, 6, 5}
    };

    build(positions, polygons);
}

void Mesh::build(const std::vector<glm::vec3> &positions,
                 const std::vector<std::vector<int>> &polygons)
{
    halfEdges.clear();
    faces.clear();
    vertices.clear();

    for(const glm::vec3 &position : positions) {
        vertices.push_back(std::make_unique<Vertex>(position));
    }

    std::map<std::pair<int, int>, HalfEdge*> edges;
    for(const std::vector<int> &polygon : polygons) {
        faces.push_back(std::make_unique<Face>());
        Face *face = faces.back().get();
        int first = halfEdges.size();
        int count = polygon.size();

        for(int i = 0; i < count; ++i) {
            halfEdges.push_back(std::make_unique<HalfEdge>());
            HalfEdge *edge = halfEdges.back().get();

            int start = polygon[i];
            int end = polygon[(i + 1) % count];
            edge->face = face;
            edge->vertex = vertices[end].get();
            edge->vertex->edge = edge;
            auto opposite = edges.find({end, start});

            if(opposite != edges.end()) {
                edge->sym = opposite->second;
                edge->sym->sym = edge;
            }

            edges[{start, end}] = edge;
        }

        for(int i = 0; i < count; ++i) {
            halfEdges[first + i]->next = halfEdges[first + (i + 1) % count].get();
        }

        face->edge = halfEdges[first].get();
    }
}

bool Mesh::loadOBJ(const QString &filename)
{
    std::ifstream file(filename.toStdString());
    if(!file) {
        return false;
    }

    std::vector<glm::vec3> positions;
    std::vector<std::vector<int>> polygons;
    std::string line;

    // Read vertex positions and polygon indices
    while(std::getline(file, line)) {
        std::istringstream stream(line);
        std::string type;
        stream >> type;

        if(type == "v") {
            glm::vec3 position;
            if(!(stream >> position.x >> position.y >> position.z)) {
                return false;
            }
            positions.push_back(position);
        } else if(type == "f") {
            std::vector<int> polygon;
            if(!readOBJFace(stream, int(positions.size()), polygon)) {
                return false;
            }

            polygons.push_back(polygon);
        }
    }

    if(positions.empty() || polygons.empty()) {
        return false;
    }

    // Replace mesh after full OBJ has been read
    build(positions, polygons);
    return true;
}

bool Mesh::readOBJFace(std::istringstream &stream, int vertexCount,
                       std::vector<int> &polygon)
{
    // Get position index from v v/vt or v/vt/vn entries
    std::string token;
    while(stream >> token) {
        if(token[0] == '#') {
            break;
        }
        std::istringstream indexStream(token.substr(0, token.find('/')));
        int index;
        if(!(indexStream >> index) || index == 0) {
            return false;
        }

        index = index < 0 ? vertexCount + index : index - 1;
        if(index < 0 || index >= vertexCount) {
            return false;
        }
        polygon.push_back(index);
    }
    return polygon.size() >= 3;
}

std::vector<HalfEdge*> Mesh::faceEdges(Face *face) const
{
    std::vector<HalfEdge*> edges;
    HalfEdge *edge = face->edge;

    do {
        edges.push_back(edge);
        edge = edge->next;
    } while(edge != face->edge);

    return edges;
}

void Mesh::splitEdge(HalfEdge *edge)
{
    if(!edge) {
        return;
    }

    HalfEdge *previous = edge;
    while(previous->next != edge) {
        previous = previous->next;
    }

    splitEdgeAt(edge, (previous->vertex->position + edge->vertex->position) * 0.5f);
}

void Mesh::splitEdgeAt(HalfEdge *edge, const glm::vec3 &position)
{
    vertices.push_back(std::make_unique<Vertex>(position));
    Vertex *midpoint = vertices.back().get();
    HalfEdge *opposite = edge->sym;

    halfEdges.push_back(std::make_unique<HalfEdge>());
    HalfEdge *second = halfEdges.back().get();

    second->vertex = edge->vertex;
    second->vertex->edge = second;
    second->face = edge->face;
    second->next = edge->next;
    edge->vertex = midpoint;
    edge->next = second;
    midpoint->edge = edge;

    if(opposite) {
        halfEdges.push_back(std::make_unique<HalfEdge>());
        HalfEdge *oppositeSecond = halfEdges.back().get();

        oppositeSecond->vertex = opposite->vertex;
        oppositeSecond->vertex->edge = oppositeSecond;
        oppositeSecond->face = opposite->face;
        oppositeSecond->next = opposite->next;
        opposite->vertex = midpoint;
        opposite->next = oppositeSecond;

        edge->sym = oppositeSecond;
        oppositeSecond->sym = edge;
        second->sym = opposite;
        opposite->sym = second;
    }
}

void Mesh::triangulateFace(Face *face)
{
    if(!face) {
        return;
    }
    
    std::vector<HalfEdge*> edges = faceEdges(face);
    HalfEdge *last = edges.back();

    for(size_t count = edges.size(); count > 3; --count) {
        HalfEdge *first = face->edge;
        HalfEdge *second = first->next;
        HalfEdge *third = second->next;

        faces.push_back(std::make_unique<Face>());
        Face *triangle = faces.back().get();

        triangle->color = face->color;
        halfEdges.push_back(std::make_unique<HalfEdge>());
        HalfEdge *diagonal = halfEdges.back().get();
        halfEdges.push_back(std::make_unique<HalfEdge>());
        HalfEdge *reverse = halfEdges.back().get();

        diagonal->sym = reverse;
        reverse->sym = diagonal;

        triangle->edge = first;
        first->face = triangle;
        second->face = triangle;
        diagonal->face = triangle;
        diagonal->vertex = last->vertex;
        second->next = diagonal;
        diagonal->next = first;

        reverse->face = face;
        reverse->vertex = second->vertex;
        reverse->next = third;
        last->next = reverse;
        face->edge = reverse;
    }
}

void Mesh::createFacePoints(std::unordered_map<Face*, Vertex*> &facePoints)
{
    for(const auto &face : faces) {
        std::vector<HalfEdge*> edges = faceEdges(face.get());
        glm::vec3 center(0.f);

        for(HalfEdge *edge : edges) {
            center += edge->vertex->position;
        }

        center /= float(edges.size());
        vertices.push_back(std::make_unique<Vertex>(center));
        facePoints[face.get()] = vertices.back().get();
    }
}

void Mesh::computeEdgePoints(const std::unordered_map<Face*, Vertex*> &facePoints,
                             std::unordered_map<HalfEdge*, glm::vec3> &edgePoints)
{
    for(const auto &face : faces) {
        for(HalfEdge *edge : faceEdges(face.get())) {
            HalfEdge *next = edge->next;
            glm::vec3 point = edge->vertex->position + next->vertex->position;

            if(next->sym) {
                point += facePoints.at(face.get())->position;
                point += facePoints.at(next->sym->face)->position;
                point *= 0.25f;
            } else {
                point *= 0.5f;
            }

            edgePoints[next] = point;
        }
    }
}

void Mesh::smoothVertices(const std::unordered_map<Face*, Vertex*> &facePoints,
                          const std::unordered_map<HalfEdge*, glm::vec3> &edgePoints)
{
    std::unordered_map<Vertex*, glm::vec3> sums;
    std::unordered_map<Vertex*, int> counts;
    std::unordered_map<Vertex*, glm::vec3> boundarySums;
    std::unordered_map<Vertex*, int> boundaryCounts;

    for(const auto &face : faces) {
        for(HalfEdge *edge : faceEdges(face.get())) {
            Vertex *vertex = edge->vertex;

            if(counts[vertex]++ == 0) {
                sums[vertex] = glm::vec3(0.f);
            }

            sums[vertex] += edgePoints.at(edge) + facePoints.at(face.get())->position;
            HalfEdge *next = edge->next;

            if(!next->sym) {
                Vertex *neighbor = next->vertex;
                if(boundaryCounts[vertex]++ == 0) {
                    boundarySums[vertex] = glm::vec3(0.f);
                }

                if(boundaryCounts[neighbor]++ == 0) {
                    boundarySums[neighbor] = glm::vec3(0.f);
                }

                boundarySums[vertex] += neighbor->position;
                boundarySums[neighbor] += vertex->position;
            }
        }
    }

    for(const auto &entry : counts) {
        Vertex *vertex = entry.first;
        float n = entry.second;

        if(boundaryCounts[vertex] == 2) {
            vertex->position = 0.75f * vertex->position + 0.125f * boundarySums.at(vertex);
        } else {
            vertex->position = (n - 2.f) / n * vertex->position + sums.at(vertex) / (n * n);
        }
    }
}

void Mesh::quadrangulateFace(Face *face, Vertex *center)
{
    std::vector<HalfEdge*> edges = faceEdges(face);
    size_t count = edges.size() / 2;
    std::vector<HalfEdge*> inward, outward;

    for(size_t i = 0; i < count; ++i) {
        halfEdges.push_back(std::make_unique<HalfEdge>());
        inward.push_back(halfEdges.back().get());
        halfEdges.push_back(std::make_unique<HalfEdge>());
        outward.push_back(halfEdges.back().get());
    }

    for(size_t i = 0; i < count; ++i) {
        Face *quad = face;
        if(i > 0) {
            faces.push_back(std::make_unique<Face>());
            quad = faces.back().get();
            quad->color = face->color;
        }

        HalfEdge *first = edges[2 * i + 1];
        HalfEdge *second = edges[(2 * i + 2) % edges.size()];

        quad->edge = first;
        first->next = second;
        second->next = inward[i];
        inward[i]->next = outward[i];
        outward[i]->next = first;

        first->face = second->face = inward[i]->face = outward[i]->face = quad;
        inward[i]->vertex = center;
        outward[i]->vertex = edges[2 * i]->vertex;
        inward[i]->sym = outward[(i + 1) % count];
        outward[(i + 1) % count]->sym = inward[i];
    }
    center->edge = inward[0];
}

void Mesh::subdivide()
{
    std::unordered_map<Face*, Vertex*> facePoints;
    std::unordered_map<HalfEdge*, glm::vec3> edgePoints;
    createFacePoints(facePoints);
    computeEdgePoints(facePoints, edgePoints);
    smoothVertices(facePoints, edgePoints);

    size_t edgeCount = halfEdges.size();
    for(size_t i = 0; i < edgeCount; ++i) {
        HalfEdge *edge = halfEdges[i].get();
        auto point = edgePoints.find(edge);

        if(point == edgePoints.end()) {
            continue;
        }

        glm::vec3 position = point->second;
        edgePoints.erase(edge->sym);
        edgePoints.erase(edge);
        splitEdgeAt(edge, position);
    }

    size_t faceCount = faces.size();
    for(size_t i = 0; i < faceCount; ++i) {
        Face *face = faces[i].get();
        quadrangulateFace(face, facePoints.at(face));
    }
}

void Mesh::extrudeFace(Face *face)
{
    if(!face) {
        return;
    }

    std::vector<HalfEdge*> edges = faceEdges(face);
    size_t count = edges.size();
    glm::vec3 normal(0.f);

    for(HalfEdge *edge : edges) {
        normal += glm::cross(edge->vertex->position, edge->next->vertex->position);
    }

    if(glm::length(normal) < 0.000001f) {
        return;
    }

    glm::vec3 offset = 0.5f * glm::normalize(normal);
    std::vector<Vertex*> bottom, top;
    std::vector<HalfEdge*> up, down;

    for(HalfEdge *edge : edges) {
        bottom.push_back(edge->vertex);
        vertices.push_back(std::make_unique<Vertex>(edge->vertex->position + offset));
        top.push_back(vertices.back().get());
    }

    for(size_t i = 0; i < count; ++i) {
        size_t previous = (i + count - 1) % count;
        faces.push_back(std::make_unique<Face>());
        Face *side = faces.back().get();
        side->color = face->color;
        HalfEdge *quad[4];

        for(int j = 0; j < 4; ++j) {
            halfEdges.push_back(std::make_unique<HalfEdge>());
            quad[j] = halfEdges.back().get();
            quad[j]->face = side;
        }

        for(int j = 0; j < 4; ++j) {
            quad[j]->next = quad[(j + 1) % 4];
        }

        side->edge = quad[0];
        quad[0]->vertex = bottom[i];
        quad[1]->vertex = top[i];
        quad[2]->vertex = top[previous];
        quad[3]->vertex = bottom[previous];
        quad[0]->sym = edges[i]->sym;

        if(quad[0]->sym) {
            quad[0]->sym->sym = quad[0];
        }

        quad[2]->sym = edges[i];
        edges[i]->sym = quad[2];
        edges[i]->vertex = top[i];
        bottom[i]->edge = quad[0];
        top[i]->edge = edges[i];
        up.push_back(quad[1]);
        down.push_back(quad[3]);
    }
    
    for(size_t i = 0; i < count; ++i) {
        up[i]->sym = down[(i + 1) % count];
        down[(i + 1) % count]->sym = up[i];
    }
}