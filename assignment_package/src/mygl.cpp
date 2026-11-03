#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      timer(), currTime(0.),
      m_geomSquare(this),
      m_progLambert(this), m_progFlat(this),
      vao(0),
      m_camera(width(), height()),
      m_mousePosPrev(),
      m_vertexDisplay(this),
      m_halfEdgeDisplay(this),
      m_faceDisplay(this),
      meshDirty(true),
      selectionDirty(true),
      mesh(this),
      selectedVertex(nullptr),
      selectedHalfEdge(nullptr),
      selectedFace(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);

    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);

    mesh.createCube();
}

MyGL::~MyGL()
{
    makeCurrent();
    mesh.destroyGPUData();
    m_vertexDisplay.destroyGPUData();
    m_halfEdgeDisplay.destroyGPUData();
    m_faceDisplay.destroyGPUData();
    if(vao != 0) {
        glDeleteVertexArrays(1, &vao);
    }
    doneCurrent();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instances of Cylinder and Sphere.
    m_geomSquare.initializeAndBufferGeometryData();

    // Create and set up the diffuse shader
    m_progLambert.createAndCompileShaderProgram(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.createAndCompileShaderProgram(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");


    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_camera.recomputeAspectRatio(w, h);

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)
    glm::mat4 viewproj = m_camera.getViewProj();
    m_progLambert.setUnifMat4("u_ViewProj", viewproj);
    m_progFlat.setUnifMat4("u_ViewProj", viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    glBindVertexArray(vao);

    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(meshDirty) {
        mesh.initializeAndBufferGeometryData();
        meshDirty = false;
    }

    if(selectionDirty) {
        m_vertexDisplay.vertex = selectedVertex;
        m_halfEdgeDisplay.edge = selectedHalfEdge;
        m_faceDisplay.face = selectedFace;
        m_vertexDisplay.initializeAndBufferGeometryData();
        m_halfEdgeDisplay.initializeAndBufferGeometryData();
        m_faceDisplay.initializeAndBufferGeometryData();
        selectionDirty = false;
    }

    glm::mat4 viewproj = m_camera.getViewProj();
    m_progLambert.setUnifMat4("u_ViewProj", viewproj);
    m_progFlat.setUnifMat4("u_ViewProj", viewproj);
    m_progLambert.setUnifVec3("u_CamPos", m_camera.eye);
    m_progFlat.setUnifMat4("u_Model", glm::mat4(1.f));

    m_progLambert.setUnifMat4("u_Model", glm::mat4(1.f));
    m_progLambert.setUnifMat4("u_ModelInvTr", glm::mat4(1.f));

    glEnable(GL_DEPTH_TEST);
    m_progLambert.draw(mesh);

    glDisable(GL_DEPTH_TEST);
    if(selectedFace) {
        m_progFlat.draw(m_faceDisplay);
    }

    if(selectedHalfEdge) {
        m_progFlat.draw(m_halfEdgeDisplay);
    }

    if(selectedVertex) {
        m_progFlat.draw(m_vertexDisplay);
    }

    glEnable(GL_DEPTH_TEST);
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_N && selectedHalfEdge) {
        selectHalfEdge(selectedHalfEdge->next);
    } else if(e->key() == Qt::Key_M && selectedHalfEdge) {
        selectHalfEdge(selectedHalfEdge->sym);
    } else if(e->key() == Qt::Key_F && selectedHalfEdge) {
        selectFace(selectedHalfEdge->face);
    } else if(e->key() == Qt::Key_V && selectedHalfEdge) {
        selectVertex(selectedHalfEdge->vertex);
    } else if(e->key() == Qt::Key_H) {
        if(e->modifiers() & Qt::ShiftModifier) {
            if(selectedFace) {
                selectHalfEdge(selectedFace->edge);
            }
        } else if(selectedVertex) {
            selectHalfEdge(selectedVertex->edge);
        }
    } else {
        OpenGLContext::keyPressEvent(e);
    }
}

void MyGL::mousePressEvent(QMouseEvent *e) {
    if(e->buttons() & (Qt::LeftButton | Qt::RightButton))
    {
        m_mousePosPrev = glm::vec2(e->pos().x(), e->pos().y());
    }
}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
    glm::vec2 pos(e->pos().x(), e->pos().y());
    if(e->buttons() & Qt::LeftButton)
    {
        // Rotation
        glm::vec2 diff = 0.2f * (pos - m_mousePosPrev);
        m_mousePosPrev = pos;
        m_camera.RotateAboutGlobalUp(-diff.x);
        m_camera.RotateAboutLocalRight(-diff.y);
    }
    else if(e->buttons() & Qt::RightButton)
    {
        // Panning
        glm::vec2 diff = 0.05f * (pos - m_mousePosPrev);
        m_mousePosPrev = pos;
        m_camera.PanAlongRight(-diff.x);
        m_camera.PanAlongUp(diff.y);
    }
    update();
}

void MyGL::wheelEvent(QWheelEvent *e) {
    m_camera.Zoom(e->angleDelta().y() * 0.001f);
    update();
}

void MyGL::tick() {
    ++currTime;
    update();
}

bool MyGL::loadOBJ(const QString &filename)
{
    if(!mesh.loadOBJ(filename)) {
        return false;
    }
    selectedVertex = nullptr;
    selectedHalfEdge = nullptr;
    selectedFace = nullptr;
    meshDirty = true;
    selectionDirty = true;
    emit selectionChanged();
    update();
    return true;
}

void MyGL::selectVertex(Vertex *vertex)
{
    selectedVertex = vertex;
    selectionDirty = true;
    emit selectionChanged();
    setFocus();
    update();
}

void MyGL::selectHalfEdge(HalfEdge *edge)
{
    selectedHalfEdge = edge;
    selectionDirty = true;
    emit selectionChanged();
    setFocus();
    update();
}

void MyGL::selectFace(Face *face)
{
    selectedFace = face;
    selectionDirty = true;
    emit selectionChanged();
    setFocus();
    update();
}

void MyGL::setVertexPosition(const glm::vec3 &position)
{
    if(selectedVertex) {
        selectedVertex->position = position;
        meshDirty = true;
        selectionDirty = true;
        update();
    }
}

void MyGL::setFaceColor(const glm::vec3 &color)
{
    if(selectedFace) {
        selectedFace->color = color;
        meshDirty = true;
        selectionDirty = true;
        update();
    }
}

void MyGL::refreshMesh()
{
    meshDirty = true;
    selectionDirty = true;
    emit selectionChanged();
    update();
}
