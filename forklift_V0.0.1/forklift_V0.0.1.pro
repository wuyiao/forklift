QT += core gui network
QT += multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#unix:LIBS += /usr/lib/x86_64-linux-gnu/libjpeg.so

#LIBS += -L/usr/local/lib \
#        -lopencv_core \
#        -lopencv_highgui \
#        -lopencv_imgproc \
#        -lopencv_videoio \
#        -lopencv_imgcodecs

#INCLUDEPATH += /usr/local/include/opencv4/

#LIBS += -L$$PWD/ffmpeglib/lib
#LIBS += -lavcodec \
#        -lavdevice \
#        -lavfilter \
#        -lavformat \
#        -lavutil \
#        -lswresample \
#        -lswscale

SOURCES += \
    LPF_V4L2.c \
    buffer_queue.c \
    qrcodegen.cpp \
    tcpServer.cpp \
#    videoCodec/videodecoder.c \
    main.cpp \
    videoWorkThread.cpp \
    widget.cpp

HEADERS += \
    LPF_V4L2.h \
    buffer_queue.h \
    common.hpp \
    qrcodegen.hpp \
    tcpServer.hpp \
#    videoCodec/videodecoder.h \
    videoWorkThread.h \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    image.qrc

DISTFILES +=
