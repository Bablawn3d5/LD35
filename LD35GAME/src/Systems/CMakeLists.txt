
set(INCROOT ${PROJECT_SOURCE_DIR}/include/Systems)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/Systems)

set(SRC
    ${SRCROOT}/TextureRenderSystem.cpp
)

set(INC
    ${INCROOT}/TextureRenderSystem.h
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
