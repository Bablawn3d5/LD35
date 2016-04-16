
set(INCROOT ${PROJECT_SOURCE_DIR}/include/Systems)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/Systems)

set(SRC
    ${SRCROOT}/LD35InputSystem.cpp
    ${SRCROOT}/TextureRenderSystem.cpp
    ${SRCROOT}/GameSystem.cpp
    ${SRCROOT}/SpawnSystem.cpp
)

set(INC
    ${INCROOT}/LD35InputSystem.h
    ${INCROOT}/TextureRenderSystem.h
    ${INCROOT}/GameSystem.h
    ${INCROOT}/SpawnSystem.h
)

# Set all source files
set(SOURCES
    ${SOURCES}
    ${SRC}
    PARENT_SCOPE
)

# Set all include files
set(INCLUDES
    ${INCLUDES}
    ${INC}
    PARENT_SCOPE
)
