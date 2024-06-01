QT += widgets network
requires(qtConfig(filedialog))

HEADERS       = mainwindow.h \
                highlighter.h \
                codeeditor.h \
                mymdichild.h
SOURCES       = main.cpp \
                mainwindow.cpp \
                highlighter.cpp \
                codeeditor.cpp \
                mymdichild.cpp
RESOURCES     = mdi.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/mainwindows/mdi
INSTALLS += target
CONFIG += console
