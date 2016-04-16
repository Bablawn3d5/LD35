cmake_minimum_required(VERSION 3.1)

project(LD35GAME)

# Default compiler args
if (CMAKE_CXX_COMPILER_ID MATCHES "(GNU|.*Clang)")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Werror -Wall -Wextra -Wno-unused-parameter -Wno-error=unused-variable -Wno-error=sign-compare -std=c++11 -DBOOST_NO_AUTO_PTR -Wno-error=deprecated-declarations -Wno-unused-local-typedefs")
    if (CMAKE_CXX_COMPILER_ID MATCHES "(.*Clang)")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers -Qunused-arguments")
    endif()
    if (CMAKE_CXX_COMPILER_ID MATCHES "(GNU)")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unused-local-typedefs")
    endif()
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-g -Os -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-g -O2 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL 'MSVC')
    # /Zi - Produces a program database (PDB) that contains type information and symbolic debugging information for use with the debugger.
    # /FS - Allows multiple cl.exe processes to write to the same .pdb file
    # /DEBUG - Enable debug during linking
    # /Od - Disables optimization
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /FS /DEBUG /Od /MDd")
    # /Ox - Full optimization
    set(CMAKE_CXX_FLAGS_RELEASE "/Ox -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Ox /Zi /FS /DEBUG")
endif()

# Define HAVE_ROUND for pymath.h not being defined by windows.
add_definitions(/DHAVE_ROUND)
# Statically link boost
add_definitions(-DBOOST_STATIC -DBOOST_PYTHON_STATIC_LIB )
# Tell CMake to build a Farquaad executable
include_directories(${Farquaad_SOURCE_DIR}/include)
include_directories(${jsoncpp-amalgamate_SOURCE_DIR})
include_directories(${EntityX_SOURCE_DIR})
include_directories(${SFML_SOURCE_DIR}/include)
include_directories(${Box2D_SOURCE_DIR})
include_directories(${thor_SOURCE_DIR}/include)
include_directories(${thor_SOURCE_DIR}/extlibs/aurora/include/)
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})

# Add target to copy files
file(GLOB ConfigLD32Files
    ${LD35GAME_SOURCE_DIR}/*.json
    ${LD35GAME_SOURCE_DIR}/Resources/*.json
    ${LD35GAME_SOURCE_DIR}/Resources/*.png)

set(INCROOT ${PROJECT_SOURCE_DIR}/include)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src)

# Include the src directory
add_subdirectory(${LD35GAME_SOURCE_DIR}/src)

# Set source_group for Visual Studio organization
foreach(FILE ${INCLUDES} ${SOURCES})
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
    get_filename_component(PARENT_DIR "${PARENT_DIR}" ABSOLUTE)

    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    # Remove redundant src\\
    string(REPLACE "src\\" "" GROUP "${GROUP}")
    string(REPLACE "\\src" "\\" GROUP "${GROUP}")

    # Group into "Source Files" and "Header Files"
    if ("${FILE}" MATCHES ".*\\.cpp")
       set(GROUP "Source Files${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.h(pp)?")
       set(GROUP "Header Files${GROUP}")
    endif()

    # Check if file exists
    if(NOT EXISTS ${FILE})
        message( FATAL_ERROR "THE FILE YOU'VE ADDED: ${FILE} DOES NOT EXIST!" )
    else()
        message ( STATUS "Group added: ${GROUP} : ${FILE}")
    endif()

    source_group("${GROUP}" FILES "${FILE}")
endforeach()

add_executable(LD35GAME main.cpp ${INCLUDES} ${SOURCES})

include_directories(${INCROOT})

add_custom_target(CopyLD32Configs ALL DEPENDS ${ConfigLD32Files})
foreach(ConfigFile ${ConfigLD32Files})
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${ConfigFile}" DIRECTORY)
    get_filename_component(PARENT_DIR "${PARENT_DIR}" ABSOLUTE)

    # Remove common directory prefix to make the group
    string(REPLACE "${LD35GAME_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    add_custom_command(TARGET CopyLD32Configs PRE_BUILD
                     COMMAND ${CMAKE_COMMAND} -E
                     make_directory "${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${GROUP}/" )
    add_custom_command(TARGET CopyLD32Configs PRE_BUILD
                     COMMAND ${CMAKE_COMMAND} -E
                         copy_if_different "${ConfigFile}"
                              "${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${GROUP}/"
                     DEPENDS ${ConfigFile}
                     COMMENT "Copying ${ConfigFile}" )
    install(FILES ${ConfigFile} DESTINATION bin/${GROUP})
endforeach()

add_dependencies(LD35GAME CopyLD32Configs)
target_link_libraries(LD35GAME Farquaad)

# Set installation directory to incl Packages
install(TARGETS LD35GAME DESTINATION bin)
if (CMAKE_BUILD_TYPE STREQUAL 'Debug')
    set(CMAKE_INSTALL_DEBUG_LIBRARIES 1)
endif()
include(InstallRequiredSystemLibraries)
