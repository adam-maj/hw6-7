#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/squareplane.h>
#include "camera.h"
#include <scene/mesh.h>
#include <scene/componentdisplay.h>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QTimer>


class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    QTimer timer;
    float currTime;

    SquarePlane m_geomSquare;// The instance of a unit cylinder we can use to render any cylinder
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera m_camera;
    // A variable used to track the mouse's previous position when
    // clicking and dragging on the GL viewport. Used to move the camera
    // in the scene.
    glm::vec2 m_mousePosPrev;

    VertexDisplay m_vertexDisplay;
    HalfEdgeDisplay m_halfEdgeDisplay;
    FaceDisplay m_faceDisplay;
    bool meshDirty;
    bool selectionDirty;


public:
    Mesh mesh;
    Vertex *selectedVertex;
    HalfEdge *selectedHalfEdge;
    Face *selectedFace;

    void selectVertex(Vertex *vertex);
    void selectHalfEdge(HalfEdge *edge);
    void selectFace(Face *face);
    void setVertexPosition(const glm::vec3 &position);
    void setFaceColor(const glm::vec3 &color);

    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    bool loadOBJ(const QString &filename);
    void refreshMesh();

protected:
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    
public slots:
    void tick();

signals:
    void selectionChanged();
};


#endif // MYGL_H
