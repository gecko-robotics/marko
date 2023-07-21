cmake_minimum_required(VERSION 3.25.0)
project(marko VERSION 2023.06.14.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(PROJECT_IS_TOP_LEVEL)
    cmake_host_system_information(RESULT HOST QUERY HOSTNAME)
    cmake_host_system_information(RESULT OSN QUERY OS_NAME)
    cmake_host_system_information(RESULT OS_VERSION QUERY OS_RELEASE)
    cmake_host_system_information(RESULT PROC QUERY PROCESSOR_DESCRIPTION)

    message(STATUS "-------------------------------------")
    message(STATUS "  Project: ${PROJECT_NAME}")
    message(STATUS "  Version: ${CMAKE_PROJECT_VERSION}")
    message(STATUS "  C++ ${CMAKE_CXX_STANDARD}")
    message(STATUS "-------------------------------------")
    message(STATUS " ${HOST}")
    message(STATUS " ${OSN}: ${OS_VERSION}")
    message(STATUS " ${PROC}")
    message(STATUS "-------------------------------------")

    set(BUILD_EXAMPLES ON)
    set(BUILD_TESTS ON)
else()
    message(STATUS "-> ${PROJECT_NAME} is submodule")
    set(BUILD_EXAMPLES OFF)
    set(BUILD_GTESTS OFF)
endif()

# Library =====================================================================
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE src/include)

# Examples ====================
message(STATUS "Building ${PROJECT_NAME} examples is ${BUILD_EXAMPLES}")
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Tests =======================
message(STATUS "Building ${PROJECT_NAME} gtests is ${BUILD_GTESTS}")
if (BUILD_TESTS)
    add_subdirectory(gtests)
endif()

# Install =====================================================================
# set(MARKO_BASE_PATH "/opt/gecko")
# set(CMAKE_INSTALL_PREFIX ${MARKO_BASE_PATH})
# set(MARKO_BIN     "${CMAKE_INSTALL_PREFIX}/bin")
# set(MARKO_LIB     "${CMAKE_INSTALL_PREFIX}/lib")
# set(MARKO_CMAKE   "${CMAKE_INSTALL_PREFIX}/lib/cmake")
# set(MARKO_INCLUDE "${CMAKE_INSTALL_PREFIX}/include/marko")

# set(CMAKE_INSTALL_PREFIX "/opt/gecko")
# include("cmake/template.cmake")
