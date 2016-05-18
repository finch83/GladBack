QT += core

CONFIG += c++11

win32 {
#QMAKE_CXXFLAGS = -Wall -Wextra -Werror
#QMAKE_CFLAGS = -Wall -Wextra -Werror
}


HEADERS += \
    zbc_gb_core.h \
    zbc_gb_log.h \
    zbc_gb_taskitembase.h

SOURCES += \
    main.cpp \
    zbc_gb_core.cpp \
    zbc_gb_log.cpp \
    zbc_gb_taskitembase.cpp

