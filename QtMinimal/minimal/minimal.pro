#-------------------------------------------------
#
# Project created by QtCreator 2012-01-19T14:34:47
#
#-------------------------------------------------

QT       += core        \
            gui         \
            opengl

TARGET = minimal
TEMPLATE = app

include(../../defines.pri)
include(../../includepath.pri)
include(../../libpath.pri)

#message($$DEFINES)
#message($$INCLUDEPATH)
#message($$LIBS)

win32-msvc* {
QMAKE_CXXFLAGS += /wd4100 /wd4101 /wd4102 /wd4189 /wd4996
}

SOURCES += main.cpp\
    ../../common/Qt4/src/SceniXQtUtil.cpp \
    ../../common/Qt4/src/SceniXQGLWidget.cpp \
    ../../common/Qt4/src/SceniXQGLSceneRendererWidget.cpp \
    ../../common/Qt4/src/SceniXQGLContext.cpp \
    ../../common/src/ui/WalkCameraManipulatorHIDSync.cpp \
    ../../common/src/ui/WalkCameraManipulator.cpp \
    ../../common/src/ui/TrackballTransformManipulatorHIDSync.cpp \
    ../../common/src/ui/TrackballTransformManipulator.cpp \
    ../../common/src/ui/TrackballCameraManipulatorHIDSync.cpp \
    ../../common/src/ui/TrackballCameraManipulator.cpp \
    ../../common/src/ui/Manipulator.cpp \
    ../../common/src/ui/FlightCameraManipulatorHIDSync.cpp \
    ../../common/src/ui/FlightCameraManipulator.cpp \
    ../../common/src/ui/CylindricalCameraManipulatorHIDSync.cpp \
    ../../common/src/ui/CylindricalCameraManipulator.cpp \
    ../../common/src/ui/AnimationManipulator.cpp \
    ../../common/src/SimpleScene.cpp \
    ../../common/src/SceniXWidget.cpp \
    ../../common/src/SceneFunctions.cpp \
    ../../common/src/MeshGenerator.cpp


HEADERS  += mainwindow.h \
    ../../common/inc/ui/WalkCameraManipulatorHIDSync.h \
    ../../common/inc/ui/WalkCameraManipulator.h \
    ../../common/inc/ui/TrackballTransformManipulatorHIDSync.h \
    ../../common/inc/ui/TrackballTransformManipulator.h \
    ../../common/inc/ui/TrackballCameraManipulatorHIDSync.h \
    ../../common/inc/ui/TrackballCameraManipulator.h \
    ../../common/inc/ui/Manipulator.h \
    ../../common/inc/ui/HumanInterfaceDevice.h \
    ../../common/inc/ui/FlightCameraManipulatorHIDSync.h \
    ../../common/inc/ui/FlightCameraManipulator.h \
    ../../common/inc/ui/CylindricalCameraManipulatorHIDSync.h \
    ../../common/inc/ui/CylindricalCameraManipulator.h \
    ../../common/inc/ui/AnimationManipulator.h \
    ../../common/inc/SimpleScene.h \
    ../../common/inc/SceniXWidget.h \
    ../../common/inc/SceneFunctions.h \
    ../../common/inc/MeshGenerator.h \
    ../../common/Qt4/inc/SceniXQtUtil.h \
    ../../common/Qt4/inc/SceniXQGLWidget.h \
    ../../common/Qt4/inc/SceniXQGLSceneRendererWidget.h \
    ../../common/Qt4/inc/SceniXQGLContext.h

