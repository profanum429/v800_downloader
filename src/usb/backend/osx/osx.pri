INCLUDEPATH += $$PWD
VPATH += $$PWD

SOURCES += rawhid_api.c
LIBS += -framework CoreFoundation -framework IOKit
