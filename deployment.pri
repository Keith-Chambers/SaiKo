unix:!android {
    isEmpty(target.path) {
        qnx {
            target.path = /tmp/$${TARGET}/bin
        } else {
#            target.path = /opt/$${TARGET}/bin
            target.path = /home/keith/Code/app_development/SaiKo/bin/
        }
        export(target.path)
    }
    INSTALLS += target
}

export(INSTALLS)
