cmake_minimum_required(VERSION 3.10.0)
project(marko VERSION 0.5.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

cmake_host_system_information(RESULT HOST QUERY HOSTNAME)
# cmake_host_system_information(RESULT CORES QUERY NUMBER_OF_LOGICAL_CORES)
cmake_host_system_information(RESULT OSN QUERY OS_NAME)
cmake_host_system_information(RESULT OS_VERSION QUERY OS_RELEASE)
cmake_host_system_information(RESULT PROC QUERY PROCESSOR_DESCRIPTION)

message(STATUS "-------------------------------------")
message(STATUS "  Project: ${PROJECT_NAME}")
message(STATUS "  C++ ${CMAKE_CXX_STANDARD}")
message(STATUS "-------------------------------------")
message(STATUS " ${HOST}")
message(STATUS " ${OSN}: ${OS_VERSION}")
message(STATUS " ${PROC}")
message(STATUS "-------------------------------------")


# Library =====================================================================
# option(BUILD_SHARED "Build dynamic shared library" ON)
# if(${BUILD_SHARED})
#     add_library(${PROJECT_NAME} SHARED $<TARGET_OBJECTS:obj>)
# else()
#     add_library(${PROJECT_NAME} STATIC $<TARGET_OBJECTS:obj>)
# endif()

# makeing an object allows us to build both shared and static
# w/o having to build library twice ... I really don't make
# use of this here :)
add_library(${PROJECT_NAME} SHARED
    src/ascii.cpp
    src/bsocket.cpp
    src/mcsocket.cpp
    src/ssocket.cpp
)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}/include
)

# Add an alias so that library can be used inside the build tree, e.g. when testing
# add_library(Marko::marko ALIAS ${PROJECT_NAME})

# Examples ====================================================================
option(EXAMPLES "Simple examples" ON)
if (EXAMPLES)
    add_subdirectory(examples)
endif()

# Install =====================================================================
# set(MARKO_BASE_PATH "/opt/gecko")
# set(CMAKE_INSTALL_PREFIX ${MARKO_BASE_PATH})
# set(MARKO_BIN     "${CMAKE_INSTALL_PREFIX}/bin")
# set(MARKO_LIB     "${CMAKE_INSTALL_PREFIX}/lib")
# set(MARKO_CMAKE   "${CMAKE_INSTALL_PREFIX}/lib/cmake")
# set(MARKO_INCLUDE "${CMAKE_INSTALL_PREFIX}/include/marko")

# set(CMAKE_INSTALL_PREFIX "/opt/gecko")
include("cmake/template.cmake")
