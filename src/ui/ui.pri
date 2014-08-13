VPATH += $$PWD

FORMS += v800main.ui \
    src/ui/v800fs.ui

macx {
ICON = src/ui/v800_downloader.icns
}

win {
RC_ICONS = src/ui/v800_downloader.ico
}
