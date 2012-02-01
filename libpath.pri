# SceniX Libs
isEmpty(SceniXHome):error("SceniXHome is not defined yet")

CONFIG(debug, debug|release):SceniXLibPath = "$$(SceniXHome)lib/x86/win/crt90/Debug"
CONFIG(release, debug|release):SceniXLibPath = "$$(SceniXHome)lib/x86/win/crt90/Release"

QTDIR_LIB = $$(QTDIR)lib

LIBS += -L$$SceniXLibPath -lSceniX9 \
        -L$$SceniXLibPath -lSceniXRT9 \
        -lopengl32 \
        -lkernel32  \
        -luser32 \
        -lgdi32 \
        -lwinspool \
        -lshell32 \
        -lole32 \
        -loleaut32 \
        -luuid \
        -lcomdlg32 \
        -ladvapi32  \

#LIBS += -L"C:/Program Files/NVIDIA Corporation/SceniX 7.2/lib/x86/win/crt90/Debug" -lSceniX9    \
#        -L"C:/Program Files/NVIDIA Corporation/SceniX 7.2/lib/x86/win/crt90/Debug" -lSceniXRT9
