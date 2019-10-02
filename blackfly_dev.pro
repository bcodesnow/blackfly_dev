QT += quick widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        imagebuffer.cpp \
        imageprocessor.cpp \
        main.cpp \
        spinnakerimaging.cpp

RESOURCES += qml.qrc


# Includes
INCLUDEPATH += /usr/include/spinnaker # spinnaker
INCLUDEPATH += /usr/local/include/opencv4 # opencv
#INCLUDEPATH += /usr/include/gstreamer-1.0 /usr/include/glib-2.0 /usr/lib/x86_64-linux-gnu/glib-2.0/include # gstreamer
# Libs
LIBS += -lSpinnaker -lSpinVideo -lSpinUpdate # spinnaker
LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio # opencv
#CONFIG += link_pkgconfig
#PKGCONFIG += gstreamer-1.0 glib-2.0 gobject-2.0  # gstreamer-app-1.0 gstreamer-pbutils-1.0 # gstreamer


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    imagebuffer.h \
    imageprocessor.h \
    spinnakerimaging.h
