QT       += core gui widgets

CONFIG += c++17
VERSION = 0.21

SOURCES += \
    CustomLayout.cpp \
    CustomLayoutItem.cpp \
    MyWidet.cpp \
    RectWidget.cpp \
    SecondScreenCanvas.cpp \
    WidgetGeometry.cpp \
    main.cpp

HEADERS += \
    CustomLayout.hpp \
    CustomLayoutItem.hpp \
    MyWidet.hpp \
    RectWidget.hpp \
    SecondScreenCanvas.hpp \
    WidgetGeometry.hpp


win32-msvc* {
    QMAKE_CXXFLAGS += /Zi /Od /RTC1
    QMAKE_CFLAGS += /Zi /Od /RTC1
}

