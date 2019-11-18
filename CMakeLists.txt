cmake_minimum_required(VERSION 3.10.0)
project(marko VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
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
set(SOURCES src/ascii.cpp src/bsocket.cpp src/mcsocket.cpp src/ssocket.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ./include
)

# Examples ====================================================================
option(EXAMPLES "Simple examples" ON)
if (EXAMPLES)
    add_subdirectory(examples)
endif()

# Install =====================================================================
set(MARKO_BASE_PATH "/opt/gecko")
# set(CMAKE_INSTALL_PREFIX ${MARKO_BASE_PATH})
set(MARKO_BIN     "${CMAKE_INSTALL_PREFIX}/bin")
set(MARKO_LIB     "${CMAKE_INSTALL_PREFIX}/lib")
set(MARKO_CMAKE   "${CMAKE_INSTALL_PREFIX}/lib/cmake")
set(MARKO_INCLUDE "${CMAKE_INSTALL_PREFIX}/include/marko")

install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION ${MARKO_LIB}  # static libraries
  LIBRARY DESTINATION ${MARKO_LIB}  # shared libraries
  RUNTIME DESTINATION ${MARKO_BIN}  # executables
)

install(DIRECTORY include/marko/
  DESTINATION ${MARKO_INCLUDE}
  FILES_MATCHING PATTERN "*.hpp"
)

install(
    FILES cmake/markoConfig.cmake
    DESTINATION ${MARKO_CMAKE}/marko
)

message(STATUS ">> Building examples: ${EXAMPLES}")
message(STATUS ">> Installing to CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
