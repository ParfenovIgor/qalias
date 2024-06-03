QT += widgets network
requires(qtConfig(filedialog))

HEADERS       = mainwindow.h \
                highlighter.h \
                codeeditor.h \
                mdichild.h
SOURCES       = main.cpp \
                mainwindow.cpp \
                highlighter.cpp \
                codeeditor.cpp \
                mdichild.cpp
RESOURCES     = qalias.qrc

# install
# target.path = $$[QT_INSTALL_EXAMPLES]/widgets/mainwindows/mdi
target.path = .
INSTALLS += target
CONFIG += console
