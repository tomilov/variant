CONFIG -= qt
CONFIG *= exceptions warn_on optimize_full c++11
TEMPLATE = app

TARGET = versatile
linux:DESTDIR = /tmp/
linux:OBJECTS_DIR = /tmp/

SOURCES += main.cpp

INCLUDEPATH += include/
HEADERS += \
    include/versatile/versatile/type_traits.hpp \
    include/versatile/versatile/recursive_wrapper.hpp \
    include/versatile/versatile/versatile.hpp \
    include/versatile/versatile/variant.hpp \
    include/versatile/versatile/apply_visitor.hpp \
    include/versatile/versatile/compare.hpp \
    include/versatile/versatile/io.hpp \
    include/versatile/versatile/visitor_utility.hpp \
    include/versatile/versatile.hpp

QMAKE_CXXFLAGS_WARN_ON = \
    -W -Weverything -pedantic -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded \
    -ftemplate-backtrace-limit=0 -fdiagnostics-color=always

#QMAKE_CXXFLAGS_RELEASE += -gline-tables-only
#QMAKE_LFLAGS_RELEASE   += -gline-tables-only
CONFIG(release, debug|release) {
    DEFINES += NDEBUG=1

    DEFINES += COLS=5 ROWS=5
}

CONFIG(debug, debug|release) {
    DEFINES += _DEBUG=1 DEBUG=1
    DEFINES += _GLIBCXX_DEBUG=1

    DEFINES += COLS=1 ROWS=1
}

