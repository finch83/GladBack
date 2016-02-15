QT += core

CONFIG += c++11

win32 {
QMAKE_CXXFLAGS = -Wall -Wextra -Werror
QMAKE_CFLAGS = -Wall -Wextra -Werror
}


HEADERS += \
    taskitembase.h

SOURCES += \
    taskitembase.cpp \
    main.cpp

