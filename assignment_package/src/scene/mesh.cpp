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
                
                if(index < 0) {
                    index += int(positions.size());
                } else {
                    --index;
                }
                
                if(index < 0 || index >= int(positions.size())) {
                    return false;
                }
                polygon.push_back(index);
            }

            if(polygon.size() < 3) {
                return false;
            }

            polygons.push_back(polygon);
        }
    }

    if(positions.empty() || polygons.empty()) {
        return false;
    }

    build(positions, polygons);
    return true;
}