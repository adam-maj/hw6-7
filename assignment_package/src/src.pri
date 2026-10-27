INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp \
    $$PWD/mygl.cpp \
    $$PWD/shaderprogram.cpp \
    $$PWD/utils.cpp \
    $$PWD/la.cpp \
    $$PWD/drawable.cpp \
    $$PWD/camera.cpp \
    $$PWD/openglcontext.cpp \
    $$PWD/scene/squareplane.cpp

HEADERS += \
    $$PWD/la.h \
    $$PWD/mainwindow.h \
    $$PWD/mygl.h \
    $$PWD/shaderprogram.h \
    $$PWD/utils.h \
    $$PWD/drawable.h \
    $$PWD/camera.h \
    $$PWD/openglcontext.h \
    $$PWD/scene/squareplane.h

SOURCES += $$PWD/scene/vertex.cpp
HEADERS += $$PWD/scene/vertex.h

SOURCES += $$PWD/scene/face.cpp
HEADERS += $$PWD/scene/face.h

SOURCES += $$PWD/scene/halfedge.cpp
HEADERS += $$PWD/scene/halfedge.h

SOURCES += $$PWD/scene/mesh.cpp
HEADERS += $$PWD/scene/mesh.h

SOURCES += $$PWD/scene/componentdisplay.cpp
HEADERS += $$PWD/scene/componentdisplay.h