#pragma once

#include "drawable.h"
#include "vertex.h"
#include "halfedge.h"
#include "face.h"

class VertexDisplay : public Drawable {
public:
    Vertex *vertex;

    VertexDisplay(OpenGLContext *context);
    void initializeAndBufferGeometryData() override;
    GLenum drawMode() const override;
};

class HalfEdgeDisplay : public Drawable {
public:
    HalfEdge *edge;

    HalfEdgeDisplay(OpenGLContext *context);
    void initializeAndBufferGeometryData() override;
    GLenum drawMode() const override;
};

class FaceDisplay : public Drawable {
public:
    Face *face;

    FaceDisplay(OpenGLContext *context);
    void initializeAndBufferGeometryData() override;
    GLenum drawMode() const override;
};