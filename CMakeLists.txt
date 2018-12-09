cmake_minimum_required (VERSION 3.0.0)

INCLUDE (GNUInstallDirs)
INCLUDE (CheckCCompilerFlag)
INCLUDE (CheckCSourceCompiles)
INCLUDE (CheckCXXCompilerFlag)
INCLUDE (CheckCXXSourceCompiles)
INCLUDE (CheckFunctionExists)
INCLUDE (CheckIncludeFile)

set (CPACK_PACKAGE_VERSION_MAJOR "0")
set (CPACK_PACKAGE_VERSION_MINOR "0")
set (CPACK_PACKAGE_VERSION_PATCH "4")

#set global Project name here
set (PROJECT_NAME "opcuawrap")
#set global project description here
set (PROJECT_DESCRIPTION "A very limited C++ wrapper for open62541")

project (
        ${PROJECT_NAME} 
        VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}
        DESCRIPTION ${PROJECT_DESCRIPTION}
        )

# Default to compiling with debug information (`gcc -g`):
if(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
	"May be one of: None Debug RelWithDebInfo Release MinSizeRel" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

#Find open62541
find_package(open62541 REQUIRED)

include(src/CMakeLists.txt)

set(SOURCE_HEADER
   ${SOURCE_HEADER}
)

# Our Common Sources
set(SOURCE
   ${SOURCE}
   ${SOURCE_HEADER}
)

# Our lib
add_library(${PROJECT_NAME} SHARED ${SOURCE})

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${SOURCE_HEADER}")

configure_file("${PROJECT_NAME}.pc.in" "${PROJECT_NAME}.pc" @ONLY)
configure_file("${PROJECT_NAME}Config.cmake.in" "${PROJECT_NAME}Config.cmake" @ONLY)

install(TARGETS ${PROJECT_NAME} 
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})


install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})


message("")
message("--------------------")
message("Configuration:")
message("   Library name and version:   " "${PROJECT_NAME}, ${PROJECT_VERSION}" )
message("   Library destination folder: " ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR} )
message("   Includes folder:            " ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME} )
message("   Pkgconfig folder:           " ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/pkgconfig )
message("   CMake folder:               " ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME} )
message("---------------------")
message("")
