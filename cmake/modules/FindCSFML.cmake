# This script locates the CSFML library
# ------------------------------------
#
# This script is a modified version of the FindSFML.cmake script from the SFML
# source tree. Basicaly, every occurrences of "sfml" is replaced with "csfml",
# and every occurrences of "hpp" is replaced with "h", for lower and upper cases.
# Modifications made by Sylvain Boilard <boilard@crans.org>.
# --------------------
#
# Usage
# -----
#
# When you try to locate the CSFML libraries, you must specify which modules you want to use (system, window, graphics, network, audio, main).
# If none is given, the CSFML_LIBRARIES variable will be empty and you'll end up linking to nothing.
# example:
#   find_package(CSFML COMPONENTS graphics window system) // find the graphics, window and system modules
#
# You can enforce a specific version, either MAJOR.MINOR or only MAJOR.
# If nothing is specified, the version won't be checked (ie. any version will be accepted).
# example:
#   find_package(CSFML COMPONENTS ...)     // no specific version required
#   find_package(CSFML 2 COMPONENTS ...)   // any 2.x version
#   find_package(CSFML 2.4 COMPONENTS ...) // version 2.4 or greater
#
# By default, the dynamic libraries of CSFML will be found. To find the static ones instead,
# you must set the CSFML_STATIC_LIBRARIES variable to TRUE before calling find_package(CSFML ...).
# In case of static linking, the CSFML_STATIC macro will also be defined by this script.
# example:
#   set(CSFML_STATIC_LIBRARIES TRUE)
#   find_package(CSFML 2 COMPONENTS network system)
#
# On Mac OS X if CSFML_STATIC_LIBRARIES is not set to TRUE then by default CMake will search for frameworks unless
# CMAKE_FIND_FRAMEWORK is set to "NEVER" for example. Please refer to CMake documentation for more details.
# Moreover, keep in mind that CSFML frameworks are only available as release libraries unlike dylibs which
# are available for both release and debug modes.
#
# If CSFML is not installed in a standard path, you can use the CSFML_ROOT CMake (or environment) variable
# to tell CMake where CSFML is.
#
# Output
# ------
#
# This script defines the following variables:
# - For each specified module XXX (system, window, graphics, network, audio, main):
#   - CSFML_XXX_LIBRARY_DEBUG:   the name of the debug library of the xxx module (set to CSFML_XXX_LIBRARY_RELEASE is no debug version is found)
#   - CSFML_XXX_LIBRARY_RELEASE: the name of the release library of the xxx module (set to CSFML_XXX_LIBRARY_DEBUG is no release version is found)
#   - CSFML_XXX_LIBRARY:         the name of the library to link to for the xxx module (includes both debug and optimized names if necessary)
#   - CSFML_XXX_FOUND:           true if either the debug or release library of the xxx module is found
# - CSFML_LIBRARIES:   the list of all libraries corresponding to the required modules
# - CSFML_FOUND:       true if all the required modules are found
# - CSFML_INCLUDE_DIR: the path where CSFML headers are located (the directory containing the CSFML/Config.h file)
#
# example:
#   find_package(CSFML 2 COMPONENTS system window graphics audio REQUIRED)
#   include_directories(${CSFML_INCLUDE_DIR})
#   add_executable(myapp ...)
#   target_link_libraries(myapp ${CSFML_LIBRARIES})

# define the CSFML_STATIC macro if static build was chosen
if(CSFML_STATIC_LIBRARIES)
    add_definitions(-DCSFML_STATIC)
endif()

# deduce the libraries suffix from the options
set(FIND_CSFML_LIB_SUFFIX "")
if(CSFML_STATIC_LIBRARIES)
    set(FIND_CSFML_LIB_SUFFIX "${FIND_CSFML_LIB_SUFFIX}-s")
endif()

# find the CSFML include directory
find_path(CSFML_INCLUDE_DIR SFML/Config.h
          PATH_SUFFIXES include
          PATHS
          ${CSFML_ROOT}
          $ENV{CSFML_ROOT}
          ~/Library/Frameworks
          /Library/Frameworks
          /usr/local/
          /usr/
          /sw          # Fink
          /opt/local/  # DarwinPorts
          /opt/csw/    # Blastwave
          /opt/)

# check the version number
set(CSFML_VERSION_OK TRUE)
if(CSFML_FIND_VERSION AND CSFML_INCLUDE_DIR)
    # extract the major and minor version numbers from CSFML/Config.h
    # we have to handle framework a little bit differently :
    if("${CSFML_INCLUDE_DIR}" MATCHES "CSFML.framework")
        set(CSFML_CONFIG_H_INPUT "${CSFML_INCLUDE_DIR}/Headers/Config.h")
    else()
        set(CSFML_CONFIG_H_INPUT "${CSFML_INCLUDE_DIR}/SFML/Config.h")
    endif()
    FILE(READ "${CSFML_CONFIG_H_INPUT}" CSFML_CONFIG_H_CONTENTS)
    STRING(REGEX MATCH ".*#define CSFML_VERSION_MAJOR ([0-9]+).*#define CSFML_VERSION_MINOR ([0-9]+).*" CSFML_CONFIG_H_CONTENTS "${CSFML_CONFIG_H_CONTENTS}")
    STRING(REGEX REPLACE ".*#define CSFML_VERSION_MAJOR ([0-9]+).*" "\\1" CSFML_VERSION_MAJOR "${CSFML_CONFIG_H_CONTENTS}")
    STRING(REGEX REPLACE ".*#define CSFML_VERSION_MINOR ([0-9]+).*" "\\1" CSFML_VERSION_MINOR "${CSFML_CONFIG_H_CONTENTS}")
    math(EXPR CSFML_REQUESTED_VERSION "${CSFML_FIND_VERSION_MAJOR} * 10 + ${CSFML_FIND_VERSION_MINOR}")

    # if we could extract them, compare with the requested version number
    if (CSFML_VERSION_MAJOR)
        # transform version numbers to an integer
        math(EXPR CSFML_VERSION "${CSFML_VERSION_MAJOR} * 10 + ${CSFML_VERSION_MINOR}")

        # compare them
        if(CSFML_VERSION LESS CSFML_REQUESTED_VERSION)
            set(CSFML_VERSION_OK FALSE)
        endif()
    else()
        # CSFML version is < 2.0
        if (CSFML_REQUESTED_VERSION GREATER 19)
            set(CSFML_VERSION_OK FALSE)
            set(CSFML_VERSION_MAJOR 1)
            set(CSFML_VERSION_MINOR x)
        endif()
    endif()
endif()

# find the requested modules
set(CSFML_FOUND TRUE) # will be set to false if one of the required modules is not found
set(FIND_CSFML_LIB_PATHS
    ${CSFML_ROOT}
    $ENV{CSFML_ROOT}
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt)
foreach(FIND_CSFML_COMPONENT ${CSFML_FIND_COMPONENTS})
    string(TOLOWER ${FIND_CSFML_COMPONENT} FIND_CSFML_COMPONENT_LOWER)
    string(TOUPPER ${FIND_CSFML_COMPONENT} FIND_CSFML_COMPONENT_UPPER)
    set(FIND_CSFML_COMPONENT_NAME csfml-${FIND_CSFML_COMPONENT_LOWER}${FIND_CSFML_LIB_SUFFIX})

    # no suffix for csfml-main, it is always a static library
    if(FIND_CSFML_COMPONENT_LOWER STREQUAL "main")
        set(FIND_CSFML_COMPONENT_NAME csfml-${FIND_CSFML_COMPONENT_LOWER})
    endif()

    # debug library
    find_library(CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_DEBUG
                 NAMES ${FIND_CSFML_COMPONENT_NAME}-d
                 PATH_SUFFIXES lib64 lib
                 PATHS ${FIND_CSFML_LIB_PATHS})

    # release library
    find_library(CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_RELEASE
                 NAMES ${FIND_CSFML_COMPONENT_NAME}
                 PATH_SUFFIXES lib64 lib
                 PATHS ${FIND_CSFML_LIB_PATHS})

    if (CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_DEBUG OR CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_RELEASE)
        # library found
        set(CSFML_${FIND_CSFML_COMPONENT_UPPER}_FOUND TRUE)

        # if both are found, set CSFML_XXX_LIBRARY to contain both
        if (CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_DEBUG AND CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_RELEASE)
            set(CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY debug     ${CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_DEBUG}
                                                          optimized ${CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_RELEASE})
        endif()

        # if only one debug/release variant is found, set the other to be equal to the found one
        if (CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_DEBUG AND NOT CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_RELEASE)
            # debug and not release
            set(CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_RELEASE ${CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_DEBUG})
            set(CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY         ${CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_DEBUG})
        endif()
        if (CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_RELEASE AND NOT CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_DEBUG)
            # release and not debug
            set(CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_DEBUG ${CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_RELEASE})
            set(CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY       ${CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_RELEASE})
        endif()
    else()
        # library not found
        set(CSFML_FOUND FALSE)
        set(CSFML_${FIND_CSFML_COMPONENT_UPPER}_FOUND FALSE)
        set(CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY "")
        set(FIND_CSFML_MISSING "${FIND_CSFML_MISSING} CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY")
    endif()

    # mark as advanced
    MARK_AS_ADVANCED(CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY
                     CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_RELEASE
                     CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY_DEBUG)

    # add to the global list of libraries
    set(CSFML_LIBRARIES ${CSFML_LIBRARIES} "${CSFML_${FIND_CSFML_COMPONENT_UPPER}_LIBRARY}")
endforeach()

# handle errors
if(NOT CSFML_VERSION_OK)
    # CSFML version not ok
    set(FIND_CSFML_ERROR "CSFML found but version too low (requested: ${CSFML_FIND_VERSION}, found: ${CSFML_VERSION_MAJOR}.${CSFML_VERSION_MINOR})")
    set(CSFML_FOUND FALSE)
elseif(NOT CSFML_FOUND)
    # include directory or library not found
    set(FIND_CSFML_ERROR "Could NOT find CSFML (missing: ${FIND_CSFML_MISSING})")
endif()
if (NOT CSFML_FOUND)
    if(CSFML_FIND_REQUIRED)
        # fatal error
        message(FATAL_ERROR ${FIND_CSFML_ERROR})
    elseif(NOT CSFML_FIND_QUIETLY)
        # error but continue
        message("${FIND_CSFML_ERROR}")
    endif()
endif()

# handle success
if(CSFML_FOUND)
    message(STATUS "Found CSFML ${CSFML_VERSION_MAJOR}.${CSFML_VERSION_MINOR} in ${CSFML_INCLUDE_DIR}")
endif()
