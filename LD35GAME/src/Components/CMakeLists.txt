
set(INCROOT ${PROJECT_SOURCE_DIR}/include/Components)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/Components)

set(SRC
    ${INCROOT}/Sprite.hpp
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
