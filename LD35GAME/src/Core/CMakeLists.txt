
set(INCROOT ${PROJECT_SOURCE_DIR}/include/Core)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/Core)

# Include hpp
set(INC
    ${INCROOT}/JSONSerializedExtendedComponents.hpp
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
