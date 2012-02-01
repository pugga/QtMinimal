# All the include paths

SceniXHome = "$$(SCENIXHOME)"
SceniXInc  = "$$(SCENIXHOME)"inc
# Include SceniX Headers
INCLUDEPATH +=  \
                $$SceniXInc                 \
                $$SceniXInc/nvsg            \
                $$SceniXInc/nvsg/nvgl       \
                $$SceniXInc/nvrt            \
                $$SceniXInc/nvrt/nvirt      \
                $$SceniXInc/nvrt/nvirtu     \

# Include SceniX+Qt4 Renderer and Manipulator
INCLUDEPATH += \
                $$PWD/common/inc            \
                $$PWD/common/Qt4/inc        \


